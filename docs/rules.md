# 比赛规则与命令

## 比赛规则

场上有两类东西：**机器人**会开火、会被打；**建筑**不会动、不会开火，只会被打。所有东西都属于红方 `red` 或蓝方 `blue`。

### 常量表

| 类型 | 命令中的名字 | 血量上限 | 初始弹量 | 单发伤害 | 单发热量 | 热量上限 | 每秒冷却 |
|---|---|---|---|---|---|---|---|
| 英雄 | `hero` | 300 | 10 | 100 | 100 | 200 | 20 |
| 步兵 | `infantry` | 200 | 100 | 10 | 10 | 100 | 10 |
| 前哨站 | `outpost` | 300 | — | — | — | — | — |
| 基地 | `base` | 600 | — | — | — | — | — |

### 规则

1. **时间**：比赛从 `t=0` 开始，单位是秒，只有 `tick` 命令会让时间前进。
2. **热量**：每发一弹，机器人热量增加“单发热量”。如果发完这一弹后热量会**超过**上限，这一弹就不能发。
3. **冷却**：时间每前进 1 秒，每台机器人的热量减少“每秒冷却”，最低减到 0。
4. **伤害**：被击中的目标血量减少“单发伤害”，最低减到 0。血量为 0 即被摧毁：机器人不能再开火，任何目标都不能再被攻击。
5. **前哨站保护基地**：本方前哨站还没被摧毁时，基地不受伤害（这一弹照常消耗弹量和热量）。
6. **胜负**：一方基地被摧毁，另一方立即获胜，比赛结束。
7. 每方最多一个前哨站、一个基地。测试保证先生成前哨站，再生成基地。

## 命令与输出

每条命令占一行，单词之间用空格分隔。需要打印时间时，格式为 `[t=当前时间]`。名字由字母、数字和下划线组成，不会重复出现。

### `spawn <名字> <类型> <阵营>`

生成一个机器人或建筑，初始血量为满血，热量为 0。

```
spawn R1 hero red
[t=0] spawn R1 hero red
```

### `shoot <开火方> <目标>`

按下面的顺序检查，遇到第一条不满足就打印对应的信息，这一弹不发出：

| 顺序 | 情况 | 输出 |
|---|---|---|
| 1 | 名字不存在 | `error: no such entity X` |
| 2 | 开火方不是机器人 | `error: X is not a robot` |
| 3 | 开火方已被摧毁 | `[t=T] X cannot shoot: dead` |
| 4 | 目标已被摧毁 | `[t=T] X cannot shoot: target down` |
| 5 | 目标是己方 | `[t=T] X cannot shoot: same team` |
| 6 | 没有弹 | `[t=T] X cannot shoot: no ammo` |
| 7 | 发出后热量超过上限 | `[t=T] X cannot shoot: overheat` |

全部通过后，弹量减 1、热量增加，然后结算伤害：

```
[t=0] R1 -> B_outpost 100 damage, hp 200     正常击中，hp 是目标剩余血量
[t=0] R1 -> B_base protected, hp 600         基地受前哨站保护
[t=0] B_outpost destroyed                    目标血量降到 0 时，紧接着再打一行
[t=0] winner: red                            被摧毁的是基地时，再打一行
```

### `tick <秒数>`

时间前进若干秒，并对所有机器人结算冷却。

```
tick 5
[t=5] tick 5
```

### `status`

先打印所有机器人，再打印所有建筑，两组内部都按名字的字典序排列：

```
B1 infantry blue hp 200/200 ammo 99 heat 10
R1 hero red hp 290/300 ammo 7 heat 200
B_base base blue hp 600/600
B_outpost outpost blue hp 100/300
```

### `report`

按名字的字典序打印每台机器人的战绩。`damage` 是累计**实际**造成的伤害（目标真正减少的血量），`kills` 是由它打出最后一击摧毁的目标数，机器人和建筑都算：

```
B1 damage 10 kills 0
R1 damage 400 kills 1
```

### 比赛结束后

有一方获胜后，`spawn`、`shoot`、`tick` 都不再执行，只打印 `error: game over`。`status` 和 `report` 照常工作。

### 完整示例

输入：

```
spawn R1 hero red
spawn B1 infantry blue
spawn B_outpost outpost blue
spawn B_base base blue
shoot R1 B_base
shoot R1 B_outpost
shoot R1 B_outpost
tick 5
shoot R1 B_outpost
shoot B1 R1
status
```

输出：

```
[t=0] spawn R1 hero red
[t=0] spawn B1 infantry blue
[t=0] spawn B_outpost outpost blue
[t=0] spawn B_base base blue
[t=0] R1 -> B_base protected, hp 600
[t=0] R1 -> B_outpost 100 damage, hp 200
[t=0] R1 cannot shoot: overheat
[t=5] tick 5
[t=5] R1 -> B_outpost 100 damage, hp 100
[t=5] B1 -> R1 10 damage, hp 290
B1 infantry blue hp 200/200 ammo 99 heat 10
R1 hero red hp 290/300 ammo 7 heat 200
B_base base blue hp 600/600
B_outpost outpost blue hp 100/300
```

第 3 次开火时 R1 热量已经是 200，再发一弹会变成 300，超过上限 200，所以不能开火。`tick 5` 冷却了 5 × 20 = 100，热量降到 100，于是又能发一弹。
