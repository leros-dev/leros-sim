import argparse
import sys
import subprocess
import os
from testfunctions import *


class DriverOptions:
    llvmPath = ""
    simExecutable = ""
    testPath = ""

class testSpec:
    testFunctor = ""
    rangeStart = 0;
    rangeEnd = 0;
    interval = 0;
    testFile = ""

SPECIAL_REGISTERS = ["ACC", "ADDR", "PC", "INSTRUCTIONS EXECUTED"]


class Driver:

    options = []
    def __init__(self, options):
        self.options = options
        self.scriptPath = os.path.dirname(os.path.realpath(__file__))
        self.testSpecs = self.parseTestFile(options.testFilePath)

        for spec in self.testSpecs:
            self.runTest(spec)
        return

    def parseTestFile(self, testFilepath):
        testSpecs = []
        with open(testFilepath) as f:
            c = f.readlines()
            for line in c:
                # Tokenize and remove newlines
                line = line.rstrip()
                if line.startswith('#'):
                    continue
                tokens = line.split(';')
                ts = testSpec()
                ts.testFunctor = tokens[0]
                ts.rangeStart = int(tokens[1])
                ts.rangeEnd = int(tokens[2])
                ts.interval = int(tokens[3])
                ts.testFile = os.path.join(self.scriptPath, tokens[4])
                testSpecs.append(ts)
        return testSpecs


    def getTestNames(self, file):
        filename = os.path.splitext(file)[0]
        nameMap = {}
        nameMap["ll"] = file
        nameMap["o"] = filename + ".o"
        nameMap["bin"] = filename + ".bin"
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

    def runTest(self, spec):
        print("Testing: %s" % spec.testFile)
        for i in range(spec.rangeStart, spec.rangeEnd, spec.interval):
            inputRegState = {}
            outputRegState = {}
            inputRegState[4] = i;
            outputRegState[4] = eval(spec.testFunctor + "(" + str(i) + ")")
            self.execute(spec.testFile, inputRegState, outputRegState)


    def execute(self, testPath, inputRegState, expectedRegState):
        os.chdir(os.path.dirname(os.path.realpath(testPath)))

        testNames = self.getTestNames(testPath)

        # Run compiler
        subprocess.call([os.path.join(self.options.llvmPath, "llc"), "-march=leros32", testNames["ll"], "--filetype=obj"])

        # Extract text segment
        subprocess.call([os.path.join(self.options.llvmPath, "llvm-objcopy"), testNames["o"],  "--dump-section", ".text=" + testNames["bin"]])

        # Get regstate string
        regstate = ""
        for reg in inputRegState:
            regstate += str(reg) + ":" + str(inputRegState[reg]) + ","
        if regstate.endswith(','):
            regstate = regstate[:-1]

        # Run the test with the given options:
        try:
            output = (subprocess.check_output([self.options.simExecutable + " --osmr --je --rs=\"" + regstate + "\" -f " + testNames["bin"]], shell=True))
        except subprocess.CalledProcessError as e:
            print(e.output)

        # Parse simulator output
        output = self.parseSimulatorOutput(output)

        # Verify output
        discrepancy = False
        for expectedReg in expectedRegState:
            if output[expectedReg] != expectedRegState[expectedReg]:
                discrepancy = True
                print("FAIL (ARG: %d): Discrepancy on register %d.  Expected: %d    Actual: %d" % (inputRegState[expectedReg], expectedReg, expectedRegState[expectedReg], output[expectedReg]))

        return



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