=============HOW TO READ=============
<>   : input string, write without ""
[]   : input int
""   : input string, write with ""
|    : or
#    : int or int list
*    : required
->(*): Complete arguments in the order of command required

============GPIO=============
> set gpio
gset -> gset [pin(#list)] {<on | off> | [0 - 255]}

> rst gpio
grst -> grst {[pin(#list)] | rn ->(*)} [from pin]* [to pin]*

> info gpio
gsts -> gsts {[pin(#list)] | rn ->(*)} [from pin]* [to pin]*

============FILE=============
> make file
fmk <typeFile> <nameFile.typeFile>

> write file
fwrt <nameFile.typeFile> "content"

> read file
frd <nameFile.typeFile>

> edit file
fedt {-n ->(*) | -r} <nameFile.typeFile> [index] "content"*

> remove file
frmv <nameFile.typeFile>

============CMD==============

cld -> cld <nameFile.typeFile> <name cmd>
> command load