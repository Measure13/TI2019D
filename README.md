# TI2019D(电赛2019年D题)
## 作者心语
这次模拟题做的还是相当遗憾的。原本以为除了C1倍增的分拿不到其他都可以拿满，谁知发挥部分（1）-（4）**不得人工干预**，而我们每次改变电路参数都不得不用reset键来复位我们的STM32，所以发挥部分相当于是一分都没有拿到。

不过，模拟罢了，事已既已，权当作教训，下次一定。

## 工程结构
`TI2019D_AD9833_Module`文件夹存放了我们使用的AD9833模块的相关资料。

`TI2019D_ESP32`是一个使用ESP32的rainmaker来控制AD9833的输出频率的小程序，真正做题时没有用到，但是本身还是比较有意义的，所以保留在了这里。

`TI2019D_etc`放了这个题目的题目要求和前后端电路、解题思路。我们的整个硬件都是按照`19-d.pdf`文件来的，代码中的`R5`、`R8`等等也是对应电路图中的电阻来说的。

`TI2019D_Python`放了一些有用的python程序。

**`TI2019D_STM32`是工程的主要部分**，存放了Keil工程配置文件。

`TI2019D_UARTHMI`放了串口屏的设计工程文件。
## 软件部分说明
代码中，关于幅频特性曲线那一块，上限频率的计算是有问题的，**不是-0.23f，而应该-3.0f**。但是，由于前后端调理电路本身的增益就随频率变化、ADC采样和计算后得到的电信号幅值的准确度随电信号本身幅值的增大而改善、采样频率不宜过大的限制，同时又苦于现实采用了“面向结果编程”的思想，采用了上述不合理的数值。后来者可以尝试调整和超越。

在写关于串口屏的程序的时候，我将原来写在ESP32上的串口屏模块移植到了STM32上，在使用过程中发现了很多问题。首先就是由于STM32的默认栈大小很小，跟ESP32没法比，所以很多原来写在函数内部的变量不得不移到全局变量中，但是我个人并不认为这是一种好习惯。然后，我在STM32的版本中引入了`printf`函数，而不是像原来那样先`sprintf`到数组里面，再接着处理。这主要是方便起见，但是按照网上一些人的说法，后者可能更好。不过由于使用了`printf`函数，而不是通过串口直接发送，所以，**一定要记得使用正确的串口来发送命令！！！**

## 如果它对你有帮助，请给它一个star呦！
## Star it if it really helps you!