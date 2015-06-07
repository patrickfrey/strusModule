strusModule	 {#mainpage}
===========

The project strusModule provides a module loader a mechanism to load strus core or anaylzer 
objects from dynamically loadable modules. With strusModule functions and objects 
can be added to strus without modification of the original source code.
You can create your own project for these extensions, build and deploy them separately.

How to implement extensions
---------------------------
With strusModule you can define two classes of loadable modules:
1. [storage module] (@ref strus::StorageModule)
* In a storage module you write extensions of the following type
** postings join operations
** weighting functions
** summarization functions

2. [analyzer module] (@ref strus::AnalyzerModule)
* In an analyzer module you write extensions of the following type
** document segmentation
** segment tokenization
** token normalization


How to use loadable extensions
------------------------------
The strus [module loader interface] (@ref strus::ModuleLoaderInterface) is constructed by
calling the createModuleLoader function implemented in the strus_module library.
The module loader allows you to load modules one by one. After loading all modules you can
construct the objects needed for your strus project.




