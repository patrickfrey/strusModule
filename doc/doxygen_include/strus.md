strusModule	 {#mainpage}
===========

The project strusModule provides a module loader a mechanism to load strus core or anaylzer 
objects from dynamically loadable modules. With strusModule functions and objects 
can be added to strus without modification of the original source code.
You can create your own project for these extensions and build and deploy them separately.

Types of extensions
-------------------
With strusModule you can define two classes of loadable modules:
1. [storage module] (@ref strus::StorageModule)
2. [analyzer module] (@ref strus::AnalyzerModule)

How to implement a storage module
---------------------------------
In a storage module you write extensions of the following type
* postings join operations (PostingJoinOperatorInterface)
* weighting functions (WeightingFunctionInterface)
* summarization functions (SummarizerFunctionInterface)

How to implement an analyzer module
---------------------------------
In an analyzer module you write extensions of the following type
* document segmentation (SegmenterInterface)
* segment tokenization (TokenizerFunctionInterface)
* token normalization (NormalizerFunctionInterface)
The following example shows a dummy analyzer module:
\code
#include "textwolf/xmlscanner.hpp"
#include "textwolf/cstringiterator.hpp"
\endcode


How to use loadable extensions
------------------------------
The strus [module loader interface] (@ref strus::ModuleLoaderInterface) is constructed by
calling the createModuleLoader function implemented in the strus_module library.
The module loader allows you to load modules one by one. After loading all modules you can
construct the objects needed for your strus project. The module loader provides you two
objects to do this, the StorageObjectBuilderInterface and the AnalyzerObjectBuilderInterface.






