#!/usr/bin/env bash

KERNEL="$(uname -a)"

case "$KERNEL" in
  Linux*)
    LOCAL_INTERFACE=$(ip route get 8.8.8.8 | sed -n 's/.*src \([^\ ]*\).*/\1/p')
    ;;
  Darwin*)
    LOCAL_INTERFACE=$(osascript -e "IPv4 address of (system info)")
    ;;
  *)
    (>&2 echo -e "\033[1;31mERROR: Unknown architecture.\033[0m") && exit 1
esac

if [ "$1" == "debug" ]; then
  case "$KERNEL" in
    Linux*)
      PREFIX="gdb --command=gdb_commands --args"
      ;;
    Darwin*)
      PREFIX="lldb --"
      ;;
  esac
  shift 1
else
  PREFIX=
fi

$PREFIX "./roq-client-bridge" \
  --name "bridge" \
  --client_listen_address "tcp://$LOCAL_INTERFACE:1234" \
  $@
