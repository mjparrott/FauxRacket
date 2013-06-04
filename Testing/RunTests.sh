#!/bin/sh
./Tests < Tests-Input.in > Actual-Output.out
diff Actual-Output.out Expected-Output.out
