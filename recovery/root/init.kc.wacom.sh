#!/system/bin/sh -eu

PATH=/sbin:/system/sbin:/system/bin:/system/xbin:/system/vendor/bin
export PATH

base_0031="13550|21681|31"
base_0061="13550|21681|61"
base_0062="13550|21681|62"
base_0064="13550|21681|64"
base="13536|21658|207"

loge ()
{
  /system/bin/log -t "WAC_FLASH" -p e "$@"
}


for i in 1 2 3 4
do
    information=`cat sys/class/wacom/wacom_emr/information`
    loge "${information}"

    if [ "${base}" == "${information}" ] ; then
        loge "FW VER OK"
        break
    elif [ "${base_0031}" == "${information}" ] || [ "${base_0061}" == "${information}" ] || [ "${base_0062}" == "${information}" ] || [ "${base_0064}" == "${information}" ] ; then
        loge "FW is WS0"
        break
    else
        loge "FWDL RUN!!!"
        echo 1 > sys/class/wacom/wacom_emr/fw_update
        ./system/bin/wac_flash /system/vendor/etc/W9013.hex /dev/i2c-5
        echo 0 > sys/class/wacom/wacom_emr/fw_update

        information=`cat sys/class/wacom/wacom_emr/information`
        loge "${information}"
        if [ "${base}" == "${information}" ] ; then
            loge "Success!"
            break
        fi
    fi
done

