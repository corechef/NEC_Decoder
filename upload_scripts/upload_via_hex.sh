#!/bin/bash

# Example usage on MacOS
# ./upload_scripts/upload_hex_via_usb.sh -c /opt/homebrew/Cellar/avrdude/8.0/.bottle/etc/avrdude.conf -p /dev/cu.usbserial-10 -h ./bin/main.hex
# Example usage on Ubuntu
# sudo ./upload_scripts/upload_via_hex.sh -c /etc/avrdude.conf -p /dev/ttyUSB0 -h ./bin/main.hex

# Usage message function
usage() {
    echo "Usage: $0 -c <config_file> -p <upload_port> -h <hex_file>"
    exit 1
}

# Default values
AVRDUDE=avrdude
PROCESSOR=atmega328p
PROGRAMMER=arduino
BAUD_RATE=115200

# Parse command line arguments
while getopts ":c:p:h:" opt; do
    case ${opt} in
        c)
            CONFIG_FILE=${OPTARG}
            ;;
        p)
            UPLOAD_PORT=${OPTARG}
            ;;
        h)
            HEX_PATH=$(realpath "${OPTARG}")
            ;;
        \?)
            echo "Invalid option: -$OPTARG" >&2
            usage
            ;;
        :)
            echo "Option -$OPTARG requires an argument." >&2
            usage
            ;;
    esac
done

# Check if CONFIG_FILE, UPLOAD_PORT, and HEX_PATH are set
if [ -z "$CONFIG_FILE" ] || [ -z "$UPLOAD_PORT" ] || [ -z "$HEX_PATH" ]; then
    echo "Error: Configuration file, upload port, and hex file path are required."
    usage
fi

# Set AVRDUDE flags
AVRDUDE_FLAGS="-C ${CONFIG_FILE} -v -V -p ${PROCESSOR} -c ${PROGRAMMER} -P ${UPLOAD_PORT} -b ${BAUD_RATE} -D"

# AVRDUDE write command
WRITE_HEX="-U flash:w:${HEX_PATH}:i"

# Check if the hex file exists
if [[ -f "$HEX_PATH" ]]; then
    # Enable command tracing
    set -x

    # Execute avrdude command
    $AVRDUDE $AVRDUDE_FLAGS $WRITE_HEX

    # Disable command tracing
    set +x
else
    echo "Hex file not found: $HEX_PATH"
    exit 1
fi
