环境：win10，VS2019

## boost库配置

### 安装

[Boost官网](https://www.boost.org/)

选择`download the latest release`，进去之后也可以选择其他版本，我选择的1.84.0

平台是windwos，选择zip文件下载，然后选择一个文件夹解压

解压后，双击运行文件夹内的`booststrap.bat`文件，生成编译程序`b2.exe`

在`b2.exe`所在目录下打开cmd/powershell/gitbash，执行如下命令：

```
.\b2.exe install --toolset=msvc-14.2 --build-type=complete --prefix="前置目录\boost1_84_0" link=static runtime-link=shared threading=multi debug release
```

各参数意义如下：

1. install可以更改为stage, stage表示只生成库(dll和lib), install还会生成包含头文件的include目录。一般来说用stage就可以了，我们将生成的lib和下载的源码包的include头文件夹放到项目要用的地方即可。
2. toolset 指定编译器，`gcc`用来编译生成linux用的库，`msvc-14.2（VS2019）`用来编译windows使用的库，版本号看你的编译器比如`msvc-10.0（VS2010）`，我的是`VS2019`所以是`msvc-14.2`。
3. 如果选择的是install 命令，指定生成的库文件夹要用`--prefix`，如果使用的是stage命令，需要用`--stagedir`指定。
4. `link` 表示生成动态库还是静态库，`static`表示生成lib库，`shared`表示生成dll库。
5. `runtime-link` 表示用于指定运行时链接方式为静态库还是动态库，指定为`static`就是`MT`模式，指定`shared`就是`MD`模式。`MD` 和 `MT` 是微软 Visual C++ 编译器的选项，用于指定运行时库的链接方式。这两个选项有以下区别：
   - `/MD`：表示使用多线程 DLL（Dynamic Link Library）版本的运行时库。这意味着你的应用程序将使用动态链接的运行时库（MSVCRT.dll）。这样的设置可以减小最终可执行文件的大小，并且允许应用程序与其他使用相同运行时库版本的程序共享代码和数据。
   - `/MT`：表示使用多线程静态库（Static Library）版本的运行时库。这意味着所有的运行时函数将被静态链接到应用程序中，使得应用程序不再依赖于动态链接的运行时库。这样可以确保应用程序在没有额外依赖的情况下独立运行，但可能会导致最终可执行文件的体积增大。

综上所述，boost编译生成了lib库，运行时采用md加载模式，生成的文件在prefix指定的文件中

最初生成的include和lib文件夹，可以修改一下目录结构（**可跳过**）：

```
--include
--lib

将include文件夹中的boost文件夹提到include同级目录下
新建stage文件夹，将lib文件夹放入stage文件夹
--boost
--stage
	--lib
```

### VS2019项目中配置boost库

随便新建一个vs项目，在解决方案下的具体项目里，右键`项目名`，选择`属性`

配置属性里，选择`VC++目录`，添加`包含目录`和`库目录`，分别对应上面的`boost`文件夹和`stage/lib`文件夹

粘贴下面代码，运行成功即为配置成功：

```c++
#include <iostream>
#include <string>
#include "boost/lexical_cast.hpp"
int main()
{
    using namespace std;
    cout << "Enter your weight: ";
    float weight;
    cin >> weight;
    string gain = "A 10% increase raises ";
    string wt = boost::lexical_cast<string> (weight);
    gain = gain + wt + " to ";      // string operator()
    weight = 1.1 * weight;
    gain = gain + boost::lexical_cast<string>(weight) + ".";
    cout << gain << endl;
    system("pause");
    return 0;
}
```

## jsoncpp配置

jsoncpp的配置稍微有些麻烦

[jsoncpp-Release版本](https://github.com/open-source-parsers/jsoncpp/releases)

选择`Source code(zip)`下载，和boost压缩文件一样，找一个目录解压

jsoncpp新版本不自带vs的sln项目文件，需要用其他方法：

1. 随便打开一个vs项目，左上角选择`文件(F)`，选择`打开(O)`，再选择`CMake(M)`文件，然后打开jsoncpp根目录下的`CMakeLists.txt`(和src文件夹同级的)

2. 等待`CMakeSettings.json`界面加载完毕，会出现CMake配置界面：

   ![image-20250516103844019](img\jsoncpp-CMakeSettings.png)

3. 一开始只会有一个x64-Debug，按如下配置

   ```
   配置类型: Debug
   工具集: msvc_x64_x64
   CMake变量和缓存(如果不显示就点击下面的`保存并生成CMake缓存以加载变量`):
   BUILD_SHARED_LIBS 值√ 保存到JSON√
   (其他变量默认选择不变)
   
   高级设置
   CMake生成器: Visual Studio 16 2019 Win64
   ```

4. 然后点击`+`新生成一个配置，选择`x64-Release`，其余配置和Debug设置的配置相同即可

5. 设置完毕后，在任务栏(VS2019最上面的文件那一栏)中点击`生成-全部生成`即可

生成的文件目录在`jsoncpp路径/out/build/x64-`，如果同时设置了Debug和Release，会有两个文件夹，这两个文件夹都需要：

1. 首先像boost一样新建一个目录用来存放头文件和库文件，比如`D:/libjson`，将jsoncpp文件夹中的`include`文件夹复制到`libjson`下面，然后再创建一个`lib`文件夹和`dll`文件夹

   ```
   --libjson
   	--include
   	--lib
   	--dll
   		--Debug
   		--Release
   ```

2. 打开jsoncpp中vs生成的Debug文件夹`xxx\jsoncpp-1.9.6\out\build\x64-Debug\lib\Debug`，将`jsoncpp.lib`文件复制到`libjson\lib`下，并改名为`jsoncpp_mdd.lib`

   同时，将`xxx\jsoncpp-1.9.6\out\build\x64-Debug\bin\Debug`目录下的`jsoncpp.dll`文件复制到`libjson\dll\Debug`文件夹下，不用改名

3. 同理，打开jsoncpp中vs生成的Release文件夹，将`jsconcpp.lib`复制到`libjson\lib`下，改名为`jsoncpp_md.lib`，将`jsoncpp.dll`复制到`libjson\dll\Release`文件夹下，不用改名

4. 新建一个vs项目，在项目属性中设置如下选项：

   ![image-20250516105645216](img\jsoncpp_project_setting.png)

   ![image-20250516105736370](img\jsoncpp_project_setting2.png)

项目属性是Debug就用之前jsoncpp生成的Debug版本的库，Release亦然（如链接器的输入，Debug就用mdd版本，Release就用md版本）

Windows运行库目录一定要设置，否则运行时可能报dll文件找不到的错误

运行下面的代码，生成并执行成功即为配置成功：

```c++
#include <iostream>
#include <json/json.h>
#include <json/value.h>
#include <json/reader.h>

int main()
{
    Json::Value root;
    root["id"] = 1001;
    root["data"] = "hello world";
    std::string request = root.toStyledString();
    std::cout << "request is " << request << std::endl;

    Json::Value root2;
    Json::Reader reader;
    reader.parse(request, root2);
    std::cout << "msg id is " << root2["id"] << " msg is " << root2["data"] << 		std::endl;
}
```