import argparse
import sys
import subprocess
import os


class DriverOptions:
    llvmPath = ""
    simExecutable = ""
    testPath = ""

SPECIAL_REGISTERS = ["ACC", "ADDR", "PC", "INSTRUCTIONS EXECUTED"]

def triangleNumber(n):
    s = 0;
    for i in range(1, n+1):
        s += i
    return s;

class Driver:

    options = []
    def __init__(self, options):
        self.options = options;

        self.runTriangleTest()
        return

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

    def runTriangleTest(self):
        for i in range (0, 100):
            inputRegState = {}
            outputRegState = {}
            inputRegState[4] = i;
            outputRegState[4] = triangleNumber(i)
            self.execute(self.options.testPath, inputRegState, outputRegState)

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

        print("Calling simulator with initial register state: %s" % regstate)

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
                print("FAIL: Discrepancy on register %d.  Expected: %d    Actual: %d" % (expectedReg, expectedRegState[expectedReg], output[expectedReg]))
        if not discrepancy:
            print("All registers are as expected")

        print("")
        return



if __name__ == "__main__":
    parser = argparse.ArgumentParser()

    parser.add_argument("--llp", help="Path to the LLVM tools which are to be used")
    parser.add_argument("--sim", help="Path to the simulator executable")
    parser.add_argument("--test", help="Path to the test file")

    args = parser.parse_args().__dict__

    if len(args) == 3:
        opt = DriverOptions()
        opt.llvmPath = os.path.expanduser(args["llp"])
        opt.simExecutable = os.path.expanduser(args["sim"])
        opt.testPath = os.path.expanduser(args["test"])

        driver = Driver(opt)

        sys.exit(0)

    sys.exit(1)