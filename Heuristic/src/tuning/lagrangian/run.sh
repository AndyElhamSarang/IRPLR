#!/bin/bash
# Launch the Lagrangian tuning (~21 h). Safe to run under nohup:
#   nohup ./run.sh > irace.log 2>&1 &
# Resume after an interruption with:  ./run.sh --recover
cd "$(dirname "$0")" || exit 1
IRACE=/home/andy/R/x86_64-pc-linux-gnu-library/4.3/irace/bin/irace
if [ "$1" = "--recover" ]; then
    cp irace.Rdata irace-recovery.Rdata
    exec "$IRACE" --scenario scenario.txt --recovery-file irace-recovery.Rdata
fi
if [ -f irace.Rdata ]; then
    echo "irace.Rdata exists; move it away or use --recover" >&2
    exit 1
fi
exec "$IRACE" --scenario scenario.txt
