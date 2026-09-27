#!/sbin/sh

MMC32="/sbin/mmc32"
DEVICE="/dev/block/mmcblk0"
LOGFILE="/tmp/mmc32_writeprotect.log"

log()
{
    echo "[mmc32-writeprotect] $*"
    echo "[mmc32-writeprotect] $*" >> "$LOGFILE"
}

fail()
{
    log "ERROR: $*"
    exit 1
}

log "========================================"
log "mmc32 write protection configuration"
log "========================================"

if [ ! -e "$MMC32" ]; then
    fail "$MMC32 does not exist"
fi

if [ ! -x "$MMC32" ]; then
    fail "$MMC32 is not executable"
fi

if [ ! -e "$DEVICE" ]; then
    fail "$DEVICE does not exist"
fi

if [ ! -b "$DEVICE" ]; then
    fail "$DEVICE is not a block device"
fi

log "mmc32 : $MMC32"
log "device : $DEVICE"
log ""

log "Executing:"
log "$MMC32 writeprotect user set none 0 409600 $DEVICE"

"$MMC32" writeprotect user set none 0 409600 "$DEVICE"
RESULT=$?

if [ "$RESULT" -ne 0 ]; then
    log "FAILED: exit code=$RESULT"
    log "========================================"
    exit "$RESULT"
fi

log "SUCCESS: exit code=0"
log ""

log "Executing:"
log "$MMC32 writeprotect user set none 507904 3932160 $DEVICE"

"$MMC32" writeprotect user set none 507904 3932160 "$DEVICE"
RESULT=$?

if [ "$RESULT" -ne 0 ]; then
    log "FAILED: exit code=$RESULT"
    log "========================================"
    exit "$RESULT"
fi

log "SUCCESS: exit code=0"
log ""

log "========================================"
log "All mmc32 writeprotect commands completed"
log "========================================"

exit 0
