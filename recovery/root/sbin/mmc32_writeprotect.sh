#!/sbin/sh

MMC64="/sbin/mmc64"
DEVICE="/dev/block/mmcblk0"
LOGFILE="/tmp/mmc64_writeprotect.log"

log()
{
    echo "[mmc64-writeprotect] $*"
    echo "[mmc64-writeprotect] $*" >> "$LOGFILE"
}

fail()
{
    log "ERROR: $*"
    exit 1
}

log "========================================"
log "mmc64 write protection configuration"
log "========================================"

if [ ! -e "$MMC64" ]; then
    fail "$MMC64 does not exist"
fi

if [ ! -x "$MMC64" ]; then
    fail "$MMC64 is not executable"
fi

if [ ! -e "$DEVICE" ]; then
    fail "$DEVICE does not exist"
fi

if [ ! -b "$DEVICE" ]; then
    fail "$DEVICE is not a block device"
fi

log "mmc64 : $MMC64"
log "device : $DEVICE"
log ""

log "Executing:"
log "$MMC64 writeprotect user set none 0 409600 $DEVICE"

"$MMC64" writeprotect user set none 0 409600 "$DEVICE"
RESULT=$?

if [ "$RESULT" -ne 0 ]; then
    log "FAILED: exit code=$RESULT"
    log "========================================"
    exit "$RESULT"
fi

log "SUCCESS: exit code=0"
log ""

log "Executing:"
log "$MMC64 writeprotect user set none 507904 3932160 $DEVICE"

"$MMC64" writeprotect user set none 507904 3932160 "$DEVICE"
RESULT=$?

if [ "$RESULT" -ne 0 ]; then
    log "FAILED: exit code=$RESULT"
    log "========================================"
    exit "$RESULT"
fi

log "SUCCESS: exit code=0"
log ""

log "========================================"
log "All mmc64 writeprotect commands completed"
log "========================================"

exit 0
