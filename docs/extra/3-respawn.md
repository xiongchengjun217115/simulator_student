# 附加 3：复活

**功能**：开启复活规则后，英雄和步兵被打掉一段时间会复活。

**要求**：

- 继承 `Respawnable`（在 `extensions.hpp` 里），并实现它唯一的函数 `bool advanceRespawn(int delay)`。英雄和步兵都要继承，建筑不用
- 这个函数的调用方式：输入里用 `rule respawn N` 开启复活后，`tick` 每过 1 秒，`Game` 就对每个机器人调用一次 `advanceRespawn(N)`（`tick 10` 就是调用 10 次）。没开启复活时不会调用
- `Game` 不知道机器人被摧毁了多久，所以你要在机器人里**自己加一个计数器**（比如 `deadFor_`），记录"已连续被摧毁几秒"。每次被调用时：
  - 还活着：计数器清零，返回 `false`
  - 已被摧毁：计数器加 1。还没到 `delay` 秒，返回 `false`；达到 `delay` 秒，就复活，计数器清零，返回 `true`
- 返回 `true` 之后，`Game` 会输出 `xxx respawned`；是否复活、复活后各个数值，都由你在函数里决定
- 复活后：血量回满，热量清零，弹量回到初始值（`Robot` 里可以自己加一个函数来做这件事）
- 做了附加 1 的话，复活后回到 `standard` 模式，切换到一半的状态也要清掉
- 建筑不复活

**例子**（`rule respawn 20`）：

```
[t=20] R1 destroyed        <- R1 被打掉。这一刻起，之后每过 1 秒，advanceRespawn 被调用一次，计数器 +1
[t=30] tick 10             <- 此时计数器是 10，不到 20，返回 false
[t=40] tick 10             <- 计数器到 20，复活，返回 true
[t=40] R1 respawned
```

测试的输入文件：`05_respawn`
