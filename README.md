#physiology-manager

Requires BioGears - prepared packages are available.


###Windows x64:
```
cmake -Wno-dev -G "Visual Studio 15 2017 Win64" -DCMAKE_PREFIX_PATH:PATH=C:\Program Files\boost\boost_1_70_0 -DCMAKE_PREFIX_PATH:PATH=C:\workspace\biogears\usr -DBOOST_LIBRARYDIR="C:\Program Files\boost\boost_1_70_0\stage\lib" ..
cmake --build . 
```

###Linux:
```

```
