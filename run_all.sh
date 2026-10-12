#!/usr/bin/env bash
# 用 build/simulator 跑 data/ 下所有 .in，完整输出写到 out/ 同名 .out。
# data/ 里有同名 .out（标准答案）的，会逐行对比，打印 PASS 或不一致的行。
#
# 用法: ./run_all.sh            跑全部并对比
#       ./run_all.sh 02_win     只跑某一个，直接打印到屏幕
set -uo pipefail
cd "$(dirname "$0")"

dir=${DATA_DIR:-data}    # 换目录: DATA_DIR=hidden ./run_all.sh

bin=build/simulator
[ -x "$bin" ] || { echo "先构建: cmake -S . -B build && cmake --build build" >&2; exit 1; }

if [ $# -ge 1 ]; then
    "$bin" < "$dir/$1.in"
    exit $?
fi

# 逐行对比两个文件（忽略多余空格），打印前 3 处不一致。
show_diff() {
    awk -v max=3 '
        function norm(s) { gsub(/[ \t\r]+/, " ", s); sub(/^ /, "", s); sub(/ $/, "", s); return s }
        NR == FNR { want[FNR] = $0; nw = FNR; next }
        { got[FNR] = $0; ng = FNR }
        END {
            n = nw > ng ? nw : ng
            bad = 0
            for (i = 1; i <= n; i++) {
                w = (i <= nw) ? norm(want[i]) : "(没有这一行)"
                g = (i <= ng) ? norm(got[i])  : "(没有这一行)"
                if (w == g) continue
                if (++bad <= max) {
                    printf "    第 %d 行\n", i
                    printf "      应该是: %s\n", w
                    printf "      你的是: %s\n", g
                }
            }
            if (bad > max) printf "    ……还有 %d 行不一致，完整输出见 out/ 目录\n", bad - max
        }
    ' "$1" "$2"
}

mkdir -p out
total=0
passed=0
unchecked=0
for f in "$dir"/*.in; do
    name=$(basename "$f" .in)
    "$bin" < "$f" > "out/$name.out" 2> "out/$name.err"
    code=$?

    expected="$dir/$name.out"
    if [ ! -f "$expected" ]; then
        unchecked=$((unchecked + 1))
        [ -s "out/$name.err" ] || rm -f "out/$name.err"
        continue
    fi

    total=$((total + 1))
    if [ "$code" -ne 0 ]; then
        printf '%-20s FAIL  程序异常退出（退出码 %d）\n' "$name" "$code"
        [ -s "out/$name.err" ] && sed 's/^/    /' "out/$name.err" | head -5
    elif diff -wq "$expected" "out/$name.out" > /dev/null; then
        printf '%-20s PASS\n' "$name"
        passed=$((passed + 1))
    else
        printf '%-20s FAIL\n' "$name"
        show_diff "$expected" "out/$name.out"
    fi
    [ -s "out/$name.err" ] || rm -f "out/$name.err"
done

echo
echo "通过 $passed / $total"
[ "$unchecked" -gt 0 ] && echo "另有 $unchecked 个输入没有标准答案，输出在 out/ 里，自己对着规则检查"
[ "$passed" -eq "$total" ]
