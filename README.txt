1.环境配置
QT : qt4.8.5，64位
GLUT : freeglut-2.8.1
GLEW : glew-1.12.0
GLM : glm-0.9.5.4
2.项目结构
RenderEngine文件夹 : 用于数据绘制渲染的文佳
Shaders文件夹 : GLSL文件，用于数据渲染
IO.h : 所有文件读取和保存相关
Algorithm.h : 所有数据预处理工作
Simulation.h : 所有仿真模拟流程
QAgentSimulation.h : 地图显示的界面窗口，包含了鼠标事件相应以及绘制流程
QAgentSimulationWidget.h : 项目主界面，包含了菜单栏和工具栏的配置
QDialogCreatRoad.h : 包含了主干道路构建算法逻辑
QAgentEnergyVis.h : 仿真模拟时对agent的能量进行可视化的窗口

