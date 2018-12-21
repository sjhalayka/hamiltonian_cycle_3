This is a C++ solution finder for the Traveling Santa 2018 - Prime Paths competition: 

https://www.kaggle.com/c/traveling-santa-2018-prime-paths

The code is not yet complete.

Here are some bugs:
- Doesn't check to make sure that there's enough cities for the capitols of the federal and provincial regions.
- Doesn't check to make sure that there are two distinct provincial capitols when creating the interprovincial path. Must keep track of not only shortest distance, but also second shortest distance.

To add:
- Find the intercity path within each province. This can't be tested right now because I need to obtain a solver...
- Obtain a solver that works well for n ~ 100.
