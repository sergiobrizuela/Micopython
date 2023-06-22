import usys as sys
import builtins

for name in ("usys", "__main__", "builtins"):
    sys.modules[name] = __import__(name)

import imp

builtins.imp = imp

