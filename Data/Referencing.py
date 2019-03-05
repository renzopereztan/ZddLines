nodes = ['97', '87', '98', '60', '59', '32', '31', '30', '6', '7', '8', '57', '56', '55', '54', '53', '52', '99', '100', '101', '102', '103', '104', '105', '106', '51', '50', '49', '48', '47', '11', '12', '13']
edges = []
path = []
counter = list(range(0,len(nodes)-1))

with open('Network.txt', 'r') as M:
    for m in M:
        o = m.split(" ")
        m = o[:2]
        edges.append(m)

for c in counter:
    for e in edges:
        if (nodes[c] in e and nodes[c+1] in e):
            path.append(edges.index(e))

with open('Path.txt','w') as f:
    for x in path:
        f.write(str(x) + " ")



