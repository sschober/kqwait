#!/bin/bash

if [[ $# -lt 1 ]]; then
  echo "Usage $0 <file|dir>[ <file|dir>]*"
  exit 1
fi

while file=$(./kqwait $@); do
  case "$file" in
    *.pm)
      echo "A perl module: $file"
      ;;
    *)
      echo "Ignoring $file"
      ;;
  esac
done
