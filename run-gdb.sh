#!/bin/sh
make run platform=qemu debug=off EXTRA_QEMUOPTS='-S -gdb tcp::26000' CPUS=1