import subprocess as sb
import threading
import random
import time


stav = "---------------------------OX------XO---------------------------"
executableFile = "Z:\\5_semester\\APC\\uloha6\\x64\\Release\\uloha6.exe"
timeFirst = 2
timeSecond = 20

def getMovesB(board,color):
    moves = []
    for i in range(8):
        for j in range(8):
            if(stav[i*8+j] == '-'):
                stavC = board[:]
                stavC,made = makeMove(stavC,chr(ord('A')+j)+str(i+1),color)
                if made:
                    moves.append((chr(ord('A')+j)+str(i+1),stavC))
    return moves


def getMoves(board,color):
    moves = []
    for i in range(8):
        for j in range(8):
            if(stav[i*8+j] == '-'):
                stavC = board[:]
                stavC,made = makeMove(stavC,chr(ord('A')+j)+str(i+1),color)
                if made:
                    moves.append(chr(ord('A')+j)+str(i+1))
    return moves

def randomPlayer(board,color):
    moves = getMoves(board,color)
    if moves != []:
        return random.choice(moves)
    return None;

#slaba heuristika 
def heuristika(board):
    weights = [ 4,-3,2, 2, 2, 2,-3,4,
				-3 ,-4,-1,-1,-1,-1 ,-4,-3,
				2  ,-1 ,1, 0, 0, 1, -1, 2,
				2  ,-1 ,0 , 1, 1,  0, -1,  2,
				2  ,-1 ,0 , 1, 1,  0, -1,  2,
				2  ,-1 ,1, 0, 0, 1, -1, 2,
				-3 ,-4,-1,-1,-1,-1 ,-4,-3,
				4  ,-3,2, 2, 2, 2,-3,4 ]
    blackTotal = 0
    whiteTotal = 0
    for i in range(64):
        if board[i] == 'X':
            blackTotal += weights[i]
        elif board[i] == 'O':
            whiteTotal += weights[i]

    return blackTotal - whiteTotal

def minmax(depth : int, board : int, max :bool, alpha, beta):
    if(depth == 0):
        return heuristika(board),None,None
    wow = getMovesB(board,max)
    if(max):
        maxVal = -9999999
        bestMove = ""
        bestBoard = ""
        for move,boardC in wow:
            lh = minmax(depth-1,boardC,False,alpha,beta)
            if(maxVal < lh[0]):
                maxVal = lh[0]
                bestBoard = boardC
                bestMove = move
            if(alpha < lh[0]):
                alpha = lh[0]
            if(alpha >= beta):
                break;
        return maxVal,bestMove,bestBoard
    else:
        maxVal = 9999999
        bestMove = ""
        bestBoard = ""
        for move,boardC in wow:
            lh = minmax(depth-1,boardC,True,alpha,beta)
            if(maxVal > lh[0]):
                maxVal = lh[0]
                bestBoard = boardC
                bestMove = move
            if(beta > lh[0]):
                beta = lh[0]
            if(alpha >= beta):
                break;
        return maxVal,bestMove,bestBoard


def flipInDirection(board, row, col,isBlack : bool, direction,atleastOne):
    if (row < 0 or row > 7 or col < 0 or col > 7 or board[8 * row + col] == '-' ): 
        return (False,board)
    if ((isBlack and board[row * 8 + col] == 'X') or (not isBlack and board[row * 8 + col] == 'O')):
        if (atleastOne): return (True,board)
        else: return (False,board)
    beforeInsert = board[row * 8 + col]
    board = board[:row * 8 + col] +  ('X' if isBlack else 'O') + board[row * 8 + col+1:]
    ok,board = flipInDirection(board, row + direction[0], col + direction[1], isBlack, direction, True)
    if (not ok):
        board = board[:row * 8 + col] +  beforeInsert + board[row * 8 + col+1:]
        return (False,board)

    return (True,board)



def makeMove(board,move,color : bool):
    col = ord(move[0]) - ord('A')
    row = int(move[1]) - 1
    directions = {(-1,-1),(-1,0),(-1,1),(0,-1),(0,1),(1,-1),(1,0),(1,1)}
    board = board[:row * 8 + col] +  ('X' if color else 'O') + board[row * 8 + col+1:]
    madeMove = False;
    for dire in directions:
         ok,board = flipInDirection(board,row+dire[0],col+dire[1],color,dire,False)
         if ok:
             madeMove = True
    return (board,madeMove)


def printBoard(board : str):
    print("  A B C D E F G H")
    line = ""
    for i in range(8):
        line = line + str(i+1) + " ";
        for j in range(8):
            line += stav[i*8+j] + " "
        print(line)
        line = ""


def sendCommand(process, command : bytes,time) -> str:
    t = threading.Timer(time,timeout)
    process.stdin.write(command)
    process.stdin.flush()
    t.start()
    result = process.stdout.readline().strip().decode('ascii')
    process.stdout.flush()
    t.cancel()
    return result;

def countPoints(board):
    bTotal = 0
    wTotal = 0
    for c in board:
        if(c == 'X'):
            bTotal+=1
        elif(c == 'O'):
            wTotal+=1

    print("------ SKORE -----")
    print("cierny: ",bTotal)
    print("biely: ", wTotal)
    print("vyhral: ", "cierny" if bTotal > wTotal else ("biely" if wTotal > bTotal else "vyrovnane"))
    print("------------------")

def timeout():
    print("didnt reply in time")
    exit(0)

val = input("hrac proti hracovy(0), hrac proti random pohybom (1)")
if val == '0':
    p1 = sb.Popen([executableFile],shell=True,stdout=sb.PIPE,stdin=sb.PIPE)
    p2 = sb.Popen([executableFile],shell=True,stdout=sb.PIPE,stdin=sb.PIPE)



    result = sendCommand(p1,("START B "+ str(timeFirst) + '\n').encode('ascii'),timeFirst)
    print(result) 
    print("player1 started")
    result = sendCommand(p2,("START W "+ str(timeSecond) + '\n').encode('ascii'),timeSecond)
    print(result) 
    print("player2 started")

    print("-"*10)

    printBoard(stav)
    i = 0;
    while((getMoves(stav,True) != []) or (getMoves(stav,False) != []) ):
        for p,t in [(p1,timeFirst),(p2,timeSecond)]:
            if ((getMoves(stav,i%2 == 0) == [])): 
                print("player skipped turn") 
                continue
            print("player {}'s turn".format((i%2) + 1))
            start = time.time();
            result = sendCommand(p,("MOVE "+ stav + '\n').encode('ascii'),t)
            end = time.time() - start;
            print("player made move: ",result)
            print("time: ",end);
            if(result != ''):
                i+=1
                stav,ok = makeMove(stav,result,p == p1)
                countPoints(stav)
                printBoard(stav)
                print(stav)
            if(i == 16):
                print("MIDGAME")
            if(i == 44):
                print("ENDGAME")
                


    result = sendCommand(p1,("STOP\n").encode('ascii'),timeFirst)
    print("player 1 stopped")
    result = sendCommand(p2,("STOP\n").encode('ascii'),timeFirst)
    print("player 2 stopped")

elif val == "1":
    randomPlayerColor = input("je random cierny(y/n)")
    randomPlayerColor = (randomPlayerColor == 'y')
    p1 = sb.Popen([executableFile],shell=True,stdout=sb.PIPE,stdin=sb.PIPE)
    result = sendCommand(p1,("START "+("W" if randomPlayerColor else "B")+" "+ str(timeFirst) + '\n').encode('ascii'),timeFirst)
    print(result) 
    print("player started")
    poradie = [(0,0),(p1,timeFirst)] if randomPlayerColor else [(p1,timeFirst),(0,0)]
    hasMoves = True;
    while ((getMoves(stav,True) != []) or (getMoves(stav,False) != []) ):
        for p,t in poradie:
            if(p == 0):
                result = randomPlayer(stav,randomPlayerColor)
                if result == None:
                    print("skipped turn")
                    continue;
            else:
                if(getMoves(stav,not randomPlayerColor) == []): continue;
                start = time.time();
                result = sendCommand(p,("MOVE "+ stav + '\n').encode('ascii'),t)
                end = time.time() - start;
                print("time: ",end);
            print("player made move: ",result)
            if(result != ''):
                stav,ok = makeMove(stav,result,(p == 0 and randomPlayerColor) or (p != 0 and not randomPlayerColor))
                countPoints(stav)
                printBoard(stav)

    result = sendCommand(p1,("STOP\n").encode('ascii'),timeFirst)
    print("player 1 stopped")

elif val == "2":
    randomPlayerColor = input("je do hlbky 6 cierny(y/n)")
    randomPlayerColor = (randomPlayerColor == 'y')
    p1 = sb.Popen([executableFile],shell=True,stdout=sb.PIPE,stdin=sb.PIPE)
    result = sendCommand(p1,("START "+("W" if randomPlayerColor else "B")+" "+ str(timeFirst) + '\n').encode('ascii'),timeFirst)
    print(result) 
    print("player started")
    poradie = [(0,0),(p1,timeFirst)] if randomPlayerColor else [(p1,timeFirst),(0,0)]
    hasMoves = True;
    while ((getMoves(stav,True) != []) or (getMoves(stav,False) != []) ):
        for p,t in poradie:
            if(p == 0):
                val,result,board = minmax(6,stav,randomPlayerColor,-9999999,9999999)
                if result == None:
                    print("skipped turn")
                    continue;
            else:
                if(getMoves(stav,not randomPlayerColor) == []): continue;
                start = time.time();
                result = sendCommand(p,("MOVE "+ stav + '\n').encode('ascii'),t)
                end = time.time() - start;
                print("time: ",end);
            print("player made move: ",result)
            if(result != ''):
                stav,ok = makeMove(stav,result,(p == 0 and randomPlayerColor) or (p != 0 and not randomPlayerColor))
                countPoints(stav)
                printBoard(stav)

    result = sendCommand(p1,("STOP\n").encode('ascii'),timeFirst)
    print("player stopped")

