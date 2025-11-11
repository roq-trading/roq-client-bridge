#!/usr/bin/env bash

KERNEL="$(uname -a)"

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
  --client_listen_address "tcp://localhost:1234" \
  $@
