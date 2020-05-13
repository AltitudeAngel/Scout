#!/bin/sh
DIR="$(pwd)"
if [ ! -f "$DIR/bin/arduino-cli" ]; then
  curl -fsSL https://raw.githubusercontent.com/arduino/arduino-cli/master/install.sh | sh
fi
export PATH="$PATH":$DIR/bin
arduino-cli config init
arduino-cli core update-index
arduino-cli core upgrade
arduino-cli core install arduino:avr
arduino-cli lib install ArduinoJson
arduino-cli compile --fqbn arduino:avr:nano --warnings all --build-path $DIR/bin --verbose Scout