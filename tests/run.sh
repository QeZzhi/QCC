#!/bin/sh
# Минимальные e2e-тесты QCC: компилируем файлы из tests/cases и сверяем
# код возврата и stdout с эталонами из tests/expected.
#
#   <name>.exit — ожидаемый код возврата (по умолчанию 0)
#   <name>.out  — ожидаемый stdout (если файла нет, stdout не проверяется)
#
# Для кейсов с ненулевым кодом дополнительно требуется непустой stderr.
set -u

ROOT=$(cd "$(dirname "$0")/.." && pwd)
QCC="$ROOT/build/qcc"
CASES="$ROOT/tests/cases"
EXPECTED="$ROOT/tests/expected"

if [ ! -x "$QCC" ]; then
    echo "tests: $QCC не найден, сначала выполни 'make'" >&2
    exit 1
fi

TMP=$(mktemp -d)
trap 'rm -rf "$TMP"' EXIT

passed=0
failed=0

for src in "$CASES"/*.qpp; do
    name=$(basename "$src" .qpp)
    want_exit=0
    if [ -f "$EXPECTED/$name.exit" ]; then
        want_exit=$(cat "$EXPECTED/$name.exit")
    fi

    "$QCC" "$src" >"$TMP/$name.out" 2>"$TMP/$name.err"
    got_exit=$?

    ok=1
    reason=""

    if [ "$got_exit" != "$want_exit" ]; then
        ok=0
        reason="код возврата: ожидался $want_exit, получен $got_exit"
    fi

    if [ -f "$EXPECTED/$name.out" ]; then
        if ! diff -u "$EXPECTED/$name.out" "$TMP/$name.out" >"$TMP/$name.diff"; then
            ok=0
            reason="stdout отличается от $name.out"
        fi
    fi

    if [ "$want_exit" != "0" ] && [ ! -s "$TMP/$name.err" ]; then
        ok=0
        reason="на ошибке ничего не напечатано в stderr"
    fi

    if [ "$ok" = "1" ]; then
        passed=$((passed + 1))
        echo "ok   $name"
    else
        failed=$((failed + 1))
        echo "FAIL $name ($reason)"
        if [ -s "$TMP/$name.diff" ]; then cat "$TMP/$name.diff"; fi
        if [ -s "$TMP/$name.err" ]; then sed 's/^/     stderr: /' "$TMP/$name.err"; fi
    fi
done

echo
echo "passed: $passed, failed: $failed"
[ "$failed" = "0" ]
