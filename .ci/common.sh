# Expect host is Linux/x86_64, Linux/aarch64, macOS/arm64
check_platform()
{
    MACHINE_TYPE=`uname -m`
    OS_TYPE=`uname -s`

    case "${MACHINE_TYPE}/${OS_TYPE}" in
        x86_64/Linux | aarch64/Linux | arm64/Darwin)
            ;;
        *)
            echo "Unsupported platform: ${MACHINE_TYPE}/${OS_TYPE}"
            exit 1
            ;;
    esac

}
