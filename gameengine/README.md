
# Fighter Engine - 2D Fighting Game

一个基于C++和Win32 API开发的原生2D格斗游戏引擎，类似拳皇的单机离线游戏。

## 项目结构

```
gameengine/
├── FighterEngine.sln          # VS2022解决方案文件
├── FighterEngine.vcxproj      # VS2022项目文件
├── main.cpp                   # 程序入口
├── GameApp.h/cpp              # 游戏主应用
├── engine/                    # 游戏引擎核心
│   ├── Vector2.h/cpp          # 2D向量类
│   ├── Color.h/cpp            # 颜色类
│   ├── Rect.h/cpp             # 矩形类
│   ├── Window.h/cpp           # 窗口系统
│   ├── Renderer.h/cpp         # 渲染系统
│   ├── Input.h/cpp            # 输入系统
│   ├── Time.h/cpp             # 时间系统
│   ├── Physics.h/cpp          # 物理系统
│   ├── Collision.h/cpp        # 碰撞检测系统
│   ├── Sprite.h/cpp           # 精灵系统
│   └── Animation.h/cpp        # 动画系统
└── game/                      # 游戏逻辑
    ├── Character.h/cpp        # 角色基类
    ├── Player.h/cpp           # 玩家角色
    ├── AIPlayer.h/cpp         # AI角色
    ├── FightSystem.h/cpp      # 战斗系统
    └── UI.h/cpp               # UI系统
```

## 编译和运行

### 环境要求
- Visual Studio 2022
- Windows 10/11
- C++17 支持

### 编译步骤
1. 双击 `FighterEngine.sln` 打开解决方案
2. 选择配置（Debug 或 Release）
3. 选择平台 x64
4. 按 F7 或点击 "生成解决方案"
5. 编译完成后按 F5 运行

### 也可以使用命令行编译：
```bash
# 使用MSBuild编译
msbuild FighterEngine.sln /p:Configuration=Release /p:Platform=x64

# 运行
bin\Release\FighterEngine.exe
```

## 操作说明

### 玩家1（蓝色角色）
- **A** - 向左移动
- **D** - 向右移动
- **W** - 跳跃
- **S** - 防御
- **J** - 出拳
- **K** - 出脚

### 通用按键
- **R** - 重新开始游戏
- **ESC** - 退出游戏

## 游戏特性

### 引擎特性
- ✅ 纯Win32 API实现，无第三方依赖
- ✅ 32位真彩色像素级渲染
- ✅ 精确的帧时间管理
- ✅ 基于矩形的碰撞检测
- ✅ 简单物理系统（重力、速度）

### 游戏特性
- ✅ 玩家控制 vs AI对战
- ✅ 角色移动、跳跃
- ✅ 出拳、出脚攻击
- ✅ 防御系统（消耗体力）
- ✅ 生命值和体力系统
- ✅ AI智能对手（三种难度）
- ✅ 血条UI显示
- ✅ 计时器
- ✅ 回合制胜利系统（2局胜利）
- ✅ 受击反馈

### 战斗系统
- 攻击有冷却时间
- 防御可以减少70%伤害
- 脚踢伤害比出拳高50%
- 受击会有击退效果
- 攻击有有效范围判定

## 技术细节

### 渲染系统
- 使用双缓冲（DIB段）避免闪烁
- 直接操作像素缓冲区进行绘制
- 支持像素、线条、矩形、圆形、文字绘制

### 物理系统
- 重力加速度：980像素/秒²
- 跳跃初速度：600像素/秒
- 移动速度：300像素/秒

### 碰撞检测
- AABB（轴对齐包围盒）碰撞检测
- 攻击判定区域和身体判定区域分离
- 碰撞法线和穿透深度计算

## 注意事项

1. 本项目仅支持Windows平台
2. 编译时请确保使用x64平台
3. 游戏窗口大小固定为1280x720
4. AI难度默认为中等，可在代码中修改

## 后续优化方向

- 添加更多动画帧
- 添加音效系统
- 添加更多可玩角色
- 添加场景背景
- 添加特殊技能系统
- 添加联网对战功能
- 添加粒子特效系统
- 优化AI智能

## 许可证

本项目仅供学习和研究使用。
