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

The following example shows a storage module template.
It includes the main header file "strus/strus.hpp" with all strus
core and analyzer declarations. For better built times it is 
recommended to include the needed headers separately instead of including 
them all by including "strus/strus.hpp". All parameters of the
strus::StorageModule object that declares the module entry point are
optional. You can pass 0, if no object of the parameter type is declared
in the module.

\code
#include "strus/strus.hpp"
#include "strus/private/dll_tags.hpp"

/*--- Posting join operators */
class DummyJoinOperator
	:public strus::PostingJoinOperatorInterface
{
public:
	... Implement the posting join operator interface here ...
private:
};

const strus::PostingJoinOperatorInterface* dummy_join()
{
	static const DummyJoinOperator dummy_join_obj;
	return &dummy_join_obj;
}

static const strus::PostingIteratorJoinConstructor postingJoinOperators[] =
{
	{"dummy_join", dummy_join},
	{0,0}
};

/*--- Weighting functions */
class DummyWeightingFunction
	:public strus::WeightingFunctionInterface
{
public:
	... Implement the weighting function interface here ...
private:
};

const strus::WeightingFunctionInterface* dummy_weighting()
{
	static const DummyWeightingFunction dummy_weighting_obj;
	return &dummy_weighting_obj;
}

static const strus::WeightingFunctionConstructor weightingFunctions[] =
{
	{"dummy_weighting", dummy_weighting},
	{0,0}
};


/*--- Summarizer functions */
class DummySummarizerFunction
	:public strus::SummarizerFunctionInterface
{
public:
	... Implement the summarizer function interface here ...
private:
};

const strus::SummarizerFunctionInterface* dummy_summarizer()
{
	static const DummySummarizerFunction dummy_summarizer_obj;
	return &dummy_summarizer_obj;
}

static const strus::SummarizerFunctionConstructor summarizerFunctions[] =
{
	{"dummy_summarizer", dummy_summarizer},
	{0,0}
};


/*--- Module declaration */
extern "C" DLL_PUBLIC strus::StorageModule entryPoint;

strus::StorageModule entryPoint( postingJoinOperators, weightingFunctions, summarizerFunctions);
\code


How to implement an analyzer module
---------------------------------
In an analyzer module you write extensions of the following type
* document segmentation (SegmenterInterface)
* segment tokenization (TokenizerFunctionInterface)
* token normalization (NormalizerFunctionInterface)

The following examples show two analyzer module templates.
Both include the main header file "strus/strus.hpp" with all strus
core and analyzer declarations. For better built times it is 
recommended to include the needed headers separately instead of including 
them all by including "strus/strus.hpp". All parameters of the
strus::AnalyzerModule object that declares the module entry point are
optional. You can pass 0 for arrays, if no object of the parameter type is declared
in the module and you can ommit the segmenter argument, if no segmenter is declared.

The first example shows a dummy analyzer module implementing a tokenizer and
a normalizer:
\code
#include "strus/strus.hpp"
#include "strus/private/dll_tags.hpp"

/*--- Tokenizer functions */
class DummyTokenizerFunction
	:public strus::TokenizerFunctionInterface
{
public:
	... Implement the tokenizer function interface here ...
private:
};

const strus::TokenizerFunctionInterface* dummy_tokenizer()
{
	static const DummyTokenizerFunction dummy_tokenizer_obj;
	return &dummy_tokenizer_obj;
}

static const strus::TokenizerConstructor tokenizers[] =
{
	{"dummy_tokenizer", dummy_tokenizer},
	{0,0}
};


/*--- Normalizer functions */
class DummyNormalizerFunction
	:public strus::NormalizerFunctionInterface
{
public:
	... Implement the normalizer function interface here ...
private:
};

const strus::NormalizerFunctionInterface* dummy_normalizer()
{
	static const DummyNormalizerFunction dummy_normalizer_obj;
	return &dummy_normalizer_obj;
}

static const strus::NormalizerConstructor normalizers[] =
{
	{"dummy_normalizer", dummy_normalizer},
	{0,0}
};

extern "C" DLL_PUBLIC strus::AnalyzerModule entryPoint;

strus::AnalyzerModule entryPoint( tokenizers, normalizers);
\endcode


The next example shows another analyzer module template for implementing a document segmenter.
\code
#include "strus/strus.hpp"
#include "strus/private/dll_tags.hpp"

class DummySegmenter
	:public strus::SegmenterInterface
{
public:
	... Implement the segmenter interface here ...
private:
};	

const strus::SegmenterInterface* dummy_segmenter()
{
	static const DummySegmenter dummy_segmenter_obj;
	return &dummy_segmenter_obj;
}

static const strus::SegmenterConstructor segmenter =
{
	"dummy_segmenter", dummy_segmenter
}


/*--- Module declaration */
extern "C" DLL_PUBLIC strus::AnalyzerModule entryPoint;

strus::AnalyzerModule entryPoint( segmenter, 0, 0);
\endcode


How to use loadable extensions
------------------------------
The strus [module loader interface] (@ref strus::ModuleLoaderInterface) is constructed by
calling the createModuleLoader function implemented in the strus_module library.
The module loader allows you to load modules one by one. After loading all modules you can
construct the objects needed for your strus project. The module loader provides you two
objects to do this, the StorageObjectBuilderInterface and the AnalyzerObjectBuilderInterface.




