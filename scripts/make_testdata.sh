#!/usr/bin/env bash

# Usage: ./make_testdata.sh [total_MB] [file_count]
#   defaults: 5120 MB (5 GB) split over 500 files in 10 subfolders

set -u # using an unset variable is an error

# ---- sizes ----
TOTAL_MB=${1:-5120}
COUNT=${2:-500}
PER_FILE_MB=$((TOTAL_MB / COUNT)) # whole MB per file (rounded down)

if [ "$PER_FILE_MB" -lt 1 ]; then
  echo "total_MB must be at least file_count (each file needs >= 1 MB)." >&2
  exit 1
fi
ACTUAL_MB=$((PER_FILE_MB * COUNT)) # what we really write

# A copy test needs the source AND the copy, so we need twice the size.
NEED_MB=$((ACTUAL_MB * 2))
SHM_FREE_MB=$(df -m --output=avail /dev/shm | tail -1 | tr -d ' ')
MEM_FREE_MB=$(awk '/MemAvailable/ {print int($2/1024)}' /proc/meminfo)

echo "Need ~${NEED_MB} MB (source + copy). /dev/shm free: ${SHM_FREE_MB} MB, RAM available: ${MEM_FREE_MB} MB."
if [ "$SHM_FREE_MB" -lt "$NEED_MB" ] || [ "$MEM_FREE_MB" -lt "$NEED_MB" ]; then
  echo "Not enough room. Try a smaller size, e.g.: $0 2048 200" >&2
  exit 1
fi

# ---- make the folders ----
ROOT=$(mktemp -d /dev/shm/byteflux_test.XXXXXX) || exit 1
SRC="$ROOT/testdata" # the files to copy, in dir_0 .. dir_9
DST="$ROOT/dest"     # starts empty; a copy lands in $DST/testdata
mkdir -p "$DST" "$SRC"/dir_{0..9}

# ---- always clean up: on Enter, Ctrl+C, or any exit ----
# armed only now, after $ROOT exists
cleanup() {
  trap - EXIT INT TERM # disarm, so this only runs once
  # safety check: only ever delete our own temp folder
  case "$ROOT" in
    /dev/shm/byteflux_test.*) rm -rf -- "$ROOT"; echo; echo "Deleted $ROOT" ;;
  esac
}
trap cleanup EXIT INT TERM

# ---- fill with random data ----
# file i goes into dir_(i mod 10), so files are spread over the 10 folders
echo "Writing $COUNT files of ${PER_FILE_MB} MB (${ACTUAL_MB} MB total)..."
for i in $(seq 1 "$COUNT"); do
  dd if=/dev/urandom of="$SRC/dir_$((i % 10))/file_$i.bin" \
    bs=1M count="$PER_FILE_MB" iflag=fullblock status=none \
    || { echo "stopped (Ctrl+C or write error)" >&2; exit 1; }
  printf '\r  %d / %d' "$i" "$COUNT"
done
echo

# ---- show paths, then wait ----
cat <<EOF

SOURCE:      $SRC
DESTINATION: $DST      (empty; the copy lands in $DST/testdata)

After a copy you can verify with:
  diff -rq "$SRC" "$DST/testdata" && echo IDENTICAL

EOF
read -r -p "Press Enter to delete all test data (or Ctrl+C)... "