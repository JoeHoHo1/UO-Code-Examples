def lookahead_agent(obs, config, depth=3):
    depth = estimate_depth(obs.board, depth)
    if sum(x != 0 for x in obs.board) < 2:
        return 3
    lines_by_index = create_lines_by_index()
    count_by_line = create_count_by_line(lines_by_index)
    count_by_line = compute_count_by_line(count_by_line, lines_by_index, obs.board)    
    moves, score  = lookahead(obs.board.copy(), lines_by_index, count_by_line, 
        True if obs.mark == 1 else False, depth , 1 if obs.mark == 1 else -1, [])       
    return moves[0]
def lookahead(board, lines_by_index, count_by_lines, search_max, depth, side, moves) :
    if depth == 0:
        return moves, get_heuristic_score(count_by_lines)
    color = 1 if search_max else -1
    best_score, best_moves = -1e16 if search_max else 1e16, []
    for r,c in get_valid_moves(board):
        new_board, new_moves = make_move(board, moves, color, r, c)
        new_count_by_lines = update_count_by_line(count_by_lines.copy(), lines_by_index, r*7 + c, color)
        if get_winner(new_count_by_lines) == color:
            score = (1e8-len(new_moves)) * color 
        else :
            new_moves, score = lookahead(new_board, lines_by_index, new_count_by_lines, 
                not search_max, depth-1, side, new_moves)
        if  (search_max and  score > best_score) or  (not search_max and score < best_score):
            best_score, best_moves  = score, new_moves
    #print("LEVEL = " + str(depth) + " return " + str(best_moves) + " with score " + str(best_score))
    return best_moves, best_score         
def estimate_depth(board, default_depth):
    depth = default_depth
    move_cnt = sum(x != 0 for x in board)
    if move_cnt > 25:
        depth = depth +  int((move_cnt-25) / 5)
    open_row_cnt = len(get_valid_moves(board))
    depth = depth + (7 - open_row_cnt)
    depth = min(42 - move_cnt, depth)
    return depth
def create_count_by_line(lines_by_index):
    lines = sorted({x for v in lines_by_index.values() for x in v})
    return dict(zip(lines , [0] * len(lines)))
def  update_count_by_line(count_by_lines, lines_by_index, idx, color):
    for encoded_line in lines_by_index[idx] :
        if encoded_line in count_by_lines :
            if color * count_by_lines[encoded_line] >= 0 : 
                count_by_lines[encoded_line] = count_by_lines[encoded_line] + color
            else :
                count_by_lines.pop(encoded_line, None)
    return count_by_lines
def compute_count_by_line(count_by_lines, lines_by_index, board):
    for idx in reversed(range(42)):
        if board[idx] != 0 :
            count_by_lines = update_count_by_line(count_by_lines, lines_by_index, idx, 1 if  board[idx] == 1 else -1)
    return count_by_lines
def encode_line(a,b,c,d) :
    return int(a + b*1e2 + c*1e4 + d*1e6)
def get_heuristic_score(count_by_lines) :
    scores = { -3 : -50, -2 : -10, -1 : -1, 0: 0, 1 : 1, 2 : 10, 3 : 50  }
    return sum( scores[x] for x in count_by_lines.values())
def get_winner(count_by_lines):
    return 1 if count_by_lines and max(count_by_lines.values()) == 4 else -1 if count_by_lines and min(count_by_lines.values()) == -4 else 0
def get_valid_moves(board):
    return [ (get_free_row(board, col), col) for col in range(7) if get_free_row(board, col) != -1 ]
def get_free_row(board, col):
    for row in reversed(range(6)) :
        if  board[row*7 + col] == 0:
            return row
    return -1
def make_move(board, moves, color, r, c):
    new_moves = moves.copy()
    new_moves.append(c)
    new_board = board.copy()
    new_board[r*7+c] = color
    return new_board, new_moves
def create_lines_by_index():
    lines_by_index = {}
    for i in range(42):
        lines_by_index[i] = []
    for r in range(6):
        for c in range(7):
            for direction in [ (0, 1), (1,1), (1,0), (1,-1) ]:
                dx,dy = direction
                for start in [-3, -2, -1, 0]:
                    line = []
                    for l in range(4):
                        _r = r + (start+l) * dy
                        _c = c + (start+l) * dx
                        if _r > -1 and _r < 6 and _c > -1 and _c < 7:
                            line.append( _r * 7 + _c)
                    if len(line) == 4:
                        encoded_line = encode_line(*sorted(line))
                        for idx in line:
                            if not encoded_line in lines_by_index[idx]:
                                lines_by_index[idx].append(encoded_line)                      
    return lines_by_index
def agent_depth6(obs, config):
    return lookahead_agent(obs, config, 6)
