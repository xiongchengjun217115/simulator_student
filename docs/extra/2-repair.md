# 附加 2：工程和修理

**功能**：新增兵种工程 `engineer`，不能开火，能给己方加血。

**要求**：

- 血量 250，弹量 0，其余数字都是 0
- 继承 `Robot` 和 `Repairer`（在 `extensions.hpp` 里）
- 修理一次给目标加 50 血，不超过上限。目标可以是机器人，也可以是建筑
- 修理后冷却 5 秒，冷却中不能再修
- 在 Robot 和 Building 里各加一个成员函数 int heal(int amount):血量增加 amount,但不超过最大血量,返回实际加了多少。Engineer 修理时调用它
- 注册 `"engineer"` 的代码已经写在 `extras.cpp` 的 `registerExtras` 里（注释状态），写完 `Engineer` 后把注释去掉即可，不改 `Game`

测试的输入文件：`04_repair`
