![showcase](https://github.com/tankiJong/asset-managment/raw/master/showcase.png)
## Dependency
* Graphviz: http://www.graphviz.org/Download_source.php

  you need to install gts for proper render
  On mac: `brew install --with-gts graphviz`

## Build
* I use CMake to build the whole project.
* **Important**: Include & lib dir should change when run on different OS.
* my environment detail:
  ```
  => OS: MacOS 10.12.2
  => MEM: 8 GB 1867 MHz, DDR3
  => CPU: 2.7 GHz Intel Core i5
  => LLVM:
  Apple LLVM version 8.0.0 (clang-800.0.38)
  Target: x86_64-apple-darwin16.3.0
  Thread model: posix
  InstalledDir: /Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin
  ```
## Core concept
### Asset
  
  `Asset` is the core concept for proper dealing the consistency and durability. It is a class of **multition pattern** so that any instance of the same name, generated from this class is controlled to be the same instance.
  
  this is some kind of scalability consideration. Since I can use `O(1)` time to update any one Asset in the system, if the assets became some kind of complicating one, we would still need O(1) time to update this Asset and immediately synchronize the change to the whole system without any additional effort. 

### DAG(Directed acyclic graph)
  
  It can be very easily proved that the whole dependency network is acyclic because interdependency will never exist. So I choose DAG as the core data structure to store the network. Using a `unordered_set` to collect all nodes of the graph, I implement a high performance DAG. The time complexity is estimated as followed (N means the total node amount of the graph):
  - insert a new Node: O(1)
  - remove an existing node(will also remove links related): O(N)
  - insert/remove a link between two node: O(N)
  - find a node: O(N)
  
  At the same time, I implemented several export format for the DAG, so that it can be easily persisted or transfered through net protocal.
  
  - Json
  - Raw text
  - Bot Language
  
 ## Implementation Highlight
  - My simple benchmark indicates that my program can process big data very fast after initializing it in acceptable time. 
    * environment: 13' MBP
    * Node: about 10,000
    * Edge(dependency): about 500,000
    * Initializing time(dealing with all input from the `resource.txt`): 4s
    * Time to process user instruction: immediate, can't feel
    * Output the processed data: immediate, cant feel
  - Visualization of the whole network.
    * Utilizing `Graphviz`, my program support to draw view of the DAG in many kind of format. In my program, I choose `svg`.
    * I write a simple webpage for better user experience. The page will be auto refreshed so that user can easily monitor the status of the whole system.(It's really helpful when debugging the program)
 
 ## cons which are worth discussion
   - When input is very large, user can clearly feel the initialization, which may cost about 1~2 second in above environment.
   - The visualization process is a little slow. But the `Graphviz` library causes the problem. I may choose another library or read the source to optimize it.
   - The visualization graph is displayed outside the core program. Though it is acceptable and a kind of elegant way to display the result, I think I can do better if I know the exact max size of input data, which is not given in the problem description. My initial idea is to wrapper the whole program as an node.js cpp-addon or transfer the serialized string of DAG so that I can use a web interface to draw a beautiful graph. At the same time, I might place some buttons and add some interaction points on the graph as a kind of user interface. But I soon realized that the data size is unpredicatable(problem says 'any size'), which means it will probably bigger than the V8(a Javascript Engine used by Chrome and Node.js) max heap size. Such a big data will not be able to store in the V8 engine, leading to a unexpected crash in the browser.