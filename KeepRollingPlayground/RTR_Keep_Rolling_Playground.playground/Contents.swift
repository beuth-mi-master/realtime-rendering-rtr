//: Playground - noun: a place where people can play

import Cocoa


class MatrixMaker {
    
    static var matrix:[[[Int]]] = [[[Int]]]()
    
    static func makeMatrix(elementsInRow: Int, rowsInBlock: Int, blocksInMatrix: Int, maxObstaclesInRow: Int) -> [[[Int]]] {
        
        for j in 0...blocksInMatrix-1 {
            var block: [[Int]] = [[Int]]()
            
                for i in 0...rowsInBlock-1 {
                    if(i % 2 == 0) {
                        block.append([0,0,0,0,0])
                    } else
                    {
                        var row:[Int] = [Int]()
                        
                        for _ in 0...elementsInRow-1 {
                            row.append(0)
                        }
                        
                        for _ in 0...maxObstaclesInRow-1 {
                            let randomNum:UInt32 = arc4random_uniform(5)
                            let randomInt:Int = Int(randomNum)
                            row.remove(at: randomInt)
                            row.insert(1, at: randomInt)
                        }
                        
                        block.append(row)
                        
                    }
                }
            
            let rings: [Int] = [2,3]
            for _ in 0...1 {
                let randomRowNum:UInt32 = arc4random_uniform(10)
                let randomRowInt:Int = Int(randomRowNum)
                let randomElementNum:UInt32 = arc4random_uniform(5)
                let randomElementInt:Int = Int(randomElementNum)
                let randomRingNum:UInt32 = arc4random_uniform(2)
                let randomRingInt:Int = Int(randomRingNum)
                
                block[randomRowInt].remove(at: randomElementInt)
                block[randomRowInt].insert(rings[randomRingInt], at: randomElementInt)
            }
                
            matrix.insert(block, at: j)
        }
        
        matrix.reverse();
        return matrix;
    }
    
    static func printMatrix() {
        if (self.matrix.count > 0) {
            for i in 0...matrix.count-1 {
                let index = i + 1
                print("Block " + String(index))
                for j in 0...matrix[i].count-1 {
                    print(matrix[i][j])
                }
            }
        }else {
            print("No matrix made yet. Call makeMatrix first.")
        }
    }
    
    static func exportMatrixAsJSON() {
        if (self.matrix.count > 0) {
            var jsonString = ""
            
            jsonString.append("{ \"arrayValues\" : ")
            jsonString.append("[")
            for i in 0...matrix.count-1 {
                for j in 0...matrix[i].count-1 {
                    jsonString.append("[")
                    for k in 0...matrix[i][j].count-1 {
                        let intValue = matrix[i][j][k]
                        jsonString.append(String(intValue))
                        if(k != matrix[i][j].count-1) {
                            jsonString.append(",")
                        }
                    }
                    jsonString.append("]")
                    if(i == matrix.count-1 && j == matrix[i].count-1) {
                        // != hat nicht geklappt
                    } else {
                        jsonString.append(",")
                    }
                    
                    
                }
            }
            jsonString.append("]")
            jsonString.append("}")
            
            print(jsonString)
            
            do {
                try jsonString.data(using: .utf8)?.write(to: URL(fileURLWithPath: "/Users/Felix/Documents/Uni/Master Semester I/Real Time Rendering/repo/real-time-rendering/KeepRollingPlayground/output/matrix.json"))
                
            } catch {
                print(error)
            }
        }else {
            print("No matrix made yet. Call makeMatrix first.")
        }
    }
    
    static func exportMatrixAsCSV() {
        if (self.matrix.count > 0) {
            var csvString = ""
            let nextLine = "\n"
            
            for i in 0...matrix.count-1 {
                for j in 0...matrix[i].count-1 {
                    for k in 0...matrix[i][j].count-1 {
                        let intValue = matrix[i][j][k]
                        csvString.append(String(intValue))
                        if(k != matrix[i][j].count-1) {
                            csvString.append(",")
                        }
                    }
                    csvString.append(nextLine)
                    
                }
            }
            
            print(csvString)
            
            do {
                try csvString.data(using: .utf8)?.write(to: URL(fileURLWithPath: "/Users/Felix/Documents/Uni/Master Semester I/Real Time Rendering/repo/real-time-rendering/KeepRollingPlayground/output/matrix.csv"))
                
            } catch {
                print(error)
            }
        }else {
            print("No matrix made yet. Call makeMatrix first.")
        }
    }
    
    static func exportMatrixSimple() {
        if (self.matrix.count > 0) {
            var simpleString = ""
            let nextLine = "\n"
            let tab = "\t"
            
            for i in 0...matrix.count-1 {
                for j in 0...matrix[i].count-1 {
                    for k in 0...matrix[i][j].count-1 {
                        let intValue = matrix[i][j][k]
                        simpleString.append(String(intValue))
                        if(k != matrix[i][j].count-1) {
                            simpleString.append(tab)
                        }
                    }
                    simpleString.append(nextLine)
                    
                }
            }
            
            print(simpleString)
            
            do {
                try simpleString.data(using: .utf8)?.write(to: URL(fileURLWithPath: "/Users/Felix/Documents/Uni/Master Semester I/Real Time Rendering/repo/real-time-rendering/KeepRollingPlayground/output/matrix.txt"))
                
            } catch {
                print(error)
            }
        }else {
            print("No matrix made yet. Call makeMatrix first.")
        }
    }
    
}

MatrixMaker.makeMatrix(elementsInRow: 5, rowsInBlock: 10, blocksInMatrix: 20, maxObstaclesInRow: 1);
MatrixMaker.exportMatrixSimple()
MatrixMaker.printMatrix()
