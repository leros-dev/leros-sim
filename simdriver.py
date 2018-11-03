import argparse
import sys
import subprocess
import os
from math import floor
from testfunctions import *


class DriverOptions:
    llvmPath = ""
    simExecutable = ""
    testPath = ""

class testSpec:
    argumentRanges = []
    testFile = ""
    verbose=False

SPECIAL_REGISTERS = ["ACC", "ADDR", "PC", "INSTRUCTIONS EXECUTED"]


class Driver:

    options = []
    def __init__(self, options):
        self.options = options
        self.scriptPath = os.path.dirname(os.path.realpath(__file__))
        self.testSpecs = self.parseTestFile(options.testFilePath)
        self.testnames = []

        for spec in self.testSpecs:
            self.currentTestSpec = spec
            self.iteration = 0
            self.totalIterations = 1
            self.runTest(spec)
        return

    def parseTestFile(self, testFilepath):
        testSpecs = []
        with open(testFilepath) as f:
            c = f.readlines()
            for line in c:
                # Tokenize and remove newlines
                line = line.rstrip()
                if line.startswith('#') or line == "":
                    continue
                tokens = line.split(';')
                ts = testSpec()
                ts.testFile = os.path.join(self.scriptPath, tokens[0])
                i = 1
                argumentRanges = []
                # Parse the next packets in groups of 3
                while floor(((len(tokens) - i) / 3)) > 0:
                    argumentRanges.append(range(int(tokens[i]), int(tokens[i+1]), int(tokens[i+2])))
                    i += 3
                if i < len(tokens):
                    # Parse the verbose argument
                    ts.verbose=True
                ts.argumentRanges = argumentRanges
                testSpecs.append(ts)
        return testSpecs


    def getTestNames(self, file):
        filename = os.path.splitext(file)[0]
        nameMap = {}
        nameMap["c"] = file
        nameMap["o"] = filename + ".o"
        nameMap["bin"] = filename + ".bin"
        nameMap["exec"] = filename
        return nameMap

    def parseSimulatorOutput(self, outputString):
        registerStates = {}
        for line in outputString.splitlines():
            # For now, dont parse special registers
            line = line.decode("utf-8")
            if any(reg in line for reg in SPECIAL_REGISTERS):
                continue
            pairs = filter(None, line.split(" "))
            for p in pairs:
                p = p.split(":")
                registerStates[int(p[0])] = int(p[1])
        return registerStates

    def compileTestPrograms(self, spec):
        # Get the names which will be generated
        testNames = self.getTestNames(spec.testFile)

        # Run compiler
        subprocess.call([os.path.join(self.options.llvmPath, "clang"), "--target=leros32", "-c", testNames["c"]])

        # Extract text segment
        subprocess.call([os.path.join(self.options.llvmPath, "llvm-objcopy"), testNames["o"], "--dump-section",
                         ".text=" + testNames["bin"]])

        # Compile to host system with the -DLEROS_HOST_TEST flag using g++
        subprocess.call(["g++", "-DLEROS_HOST_TEST", testNames["c"], "-o", testNames["exec"]])

    def parseHostOutput(self, executable, inputState):
        argString = ""
        for i in inputState:
            argString += str(inputState[i]) + " "
        output = subprocess.check_output("%s %s" % (executable, argString), shell=True)
        return int(output)

    def recurseRunTest(self, ranges, inputRegState, argumentIndex):
        if len(ranges) > 0:
            # Expand range through recursion
            for i in ranges[0]:
                inputRegState[argumentIndex] = i
                self.recurseRunTest(ranges[1:], inputRegState, argumentIndex+ 1)
        else:
            # No more ranges to expand, do test
            if self.currentTestSpec.verbose and self.iteration > 0 and (self.iteration % (self.totalIterations / 100)) == 0:
                s = "Test %d:%d" % (self.iteration, self.totalIterations)
                print(s)
            self.iteration += 1
            outputRegState = {}
            outputRegState[4] = self.parseHostOutput(self.testNames["exec"], inputRegState)
            return self.executeSimulator(self.testNames["c"], inputRegState, outputRegState)

    def runTest(self, spec):
        print("Testing: %s" % spec.testFile)
        self.testNames = self.getTestNames(spec.testFile)
        os.chdir(os.path.dirname(os.path.realpath(spec.testFile)))

        self.compileTestPrograms(spec)

        # Calculate the number of iterations were going to be doing
        for r in spec.argumentRanges:
            iterRange = 0
            for i in r:
                iterRange += 1 # ewwww
            self.totalIterations *= iterRange

        # Expand input arguments. We expect that the initial argument is given from register r4
        self.recurseRunTest(spec.argumentRanges, {}, 4)


        # Cleanup
        os.remove(self.testNames["exec"])
        os.remove(self.testNames["o"])
        os.remove(self.testNames["bin"])

    def executeSimulator(self, testPath, inputRegState, expectedRegState):
        # Get regstate string
        regstate = ""
        for reg in inputRegState:
            regstate += str(reg) + ":" + str(inputRegState[reg]) + ","
        if regstate.endswith(','):
            regstate = regstate[:-1]

        # Run the test with the given options:
        try:
            output = (subprocess.check_output([self.options.simExecutable + " --osmr --je --rs=\"" +
                                               regstate + "\" -f " + self.testNames["bin"]], shell=True))
        except subprocess.CalledProcessError as e:
            print(e.output)
            return True

        # Parse simulator output
        output = self.parseSimulatorOutput(output)

        # Verify output
        discrepancy = False
        for expectedReg in expectedRegState:
            if output[expectedReg] != expectedRegState[expectedReg]:
                discrepancy = True
                print("FAIL (ARG: %d): Discrepancy on register %d.  Expected: %d    Actual: %d" % (inputRegState[expectedReg], expectedReg, expectedRegState[expectedReg], output[expectedReg]))

        return discrepancy



if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("--llp", help="Path to the LLVM tools which are to be used")
    parser.add_argument("--sim", help="Path to the simulator executable")
    parser.add_argument("--test", help="Path to the test file specification")

    args = parser.parse_args()

    if len(sys.argv) > 3:
        opt = DriverOptions()
        opt.llvmPath = os.path.expanduser(args.llp)
        opt.simExecutable = os.path.expanduser(args.sim)
        opt.testFilePath = os.path.expanduser(args.test)

        driver = Driver(opt)

        sys.exit(0)

    parser.print_help()
    sys.exit(1)