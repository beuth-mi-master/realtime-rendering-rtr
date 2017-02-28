# Exercise 5

## Members
* Felix Maulwurf, 797854 (s68703@beuth-hochschule.de)
* Michael Duve, 792102 (s67186@beuth-hochschule.de)

## Matrix Maker

* functions are static no need to initialize an object
* call makeMatrix function first

| Parameters
--- | ---| ---
name | default value | description
elementsInRow | 5| How many elements do you want in a row ? Use an int > 2.
rowsInBlock | 10| How many rows do you want in a block ? Use an int % 2 = 0. For example 2, 4, 6, 8, 10.
blocksInMatrix | 5 | How many blocks do you want in the matrix ? Use an int > 1 because the last block will be a tunnel.
maxObstaclesInRow | 2 | Every second row can have obstacles. How many of them do you want (MAX) ? Their position will be random.

* call printMatrix function to see generated matrix by given parameters

* replace URL in exportMatrixToCSV function with the direct path to your output directory
* give the csv a name
* call exportMatrixToCSV function
