import sys

def draw_graph(offset, pos_list, edge_list, color_list, edge_bold_set):

    str = ""

    #color_names = ["black", "red", "blue", "green"]
    color_names = ["black", "red", "purple", "blue", "green", "pink", "brown", "greenyellow", "orange", "skyblue"]
    radius = 8

    count = 1
    for i in range(len(edge_list)):
        e = edge_list[i]
        p = pos_list[e[0] - 1]
        x1 = p[0] + offset[0] #<!--stroke-dasharray="5 5" -->
        y1 = p[1] + offset[1]
        p = pos_list[e[1] - 1]
        x2 = p[0] + offset[0]
        y2 = p[1] + offset[1]

        if color_list[i] <= 0:
            color = "black"
        else:
            color = color_names[color_list[i]]
        #if color_list[i] < 0:
        #    swidth = 2
        #    opt = 'stroke-dasharray="3 3" '
        #else:
        #    swidth = 10
        #    opt = ''
        if i in edge_bold_set:
            swidth = 14
            opt = ''
        else:
            swidth = 1
            opt = ''

        str += '<line x1="{0}" y1="{1}" x2="{2}" y2="{3}" '.format(x1, y1, x2, y2)
        str += 'stroke="{0}" stroke-width="{1}" {2}/> '.format(color, swidth, opt)
        str += "\n"

    for p in pos_list:
        x = p[0] + offset[0]
        y = p[1] + offset[1]
        str += '<circle cx="{0}" cy="{1}" r="{2}" '.format(x, y, radius)
        str += 'fill="white" stroke="black" stroke-width="2" />'
        str += "\n"

    xmax = max([p[0] for p in pos_list])
    ymax = max([p[1] for p in pos_list])
    return (str, xmax + radius, ymax + radius)

if __name__ == "__main__":

    margin = (20, 20)
    numofx = 6

    if len(sys.argv) <= 2:
        print("Usage: python {0} <network file> <pos list> <bold edge set>".format(sys.argv[0]))
        exit(0)

    edge_list = []
    color_list = []
    with open(sys.argv[1]) as f:
        for line in f:
            ar = line.strip().split()
            edge_list.append((int(ar[0]), int(ar[1])))
            color_list.append(int(ar[3]))

    num_m = len(edge_list) # number of edges
    ar = [max(e) for e in edge_list]
    num_n = max(ar) # number of vertices

    pos_list = []
    with open(sys.argv[2]) as f:
        for line in f:
            ar = line.strip().split()
            pos_list.append((int(ar[0]), int(ar[1])))

    with open(sys.argv[3]) as f:
        ar = f.read().replace("\r", "").replace("\n", " ").split()
        bold_edge_set = list(map(int, ar))

    axmax = margin[0]
    aymax = margin[1]
    astr = ""
    offset = [margin[0], margin[1]]
    count = 0

    for i in range(len(color_list)):
        if color_list[i] == 0:
            color_list[i] = -1

    (str, xmax, ymax) = draw_graph(offset, pos_list, edge_list, color_list, bold_edge_set)

    if axmax < offset[0] + xmax + margin[0]:
        axmax = offset[0] + xmax + margin[0]
    if aymax < offset[1] + ymax + margin[1]:
        aymax = offset[1] + ymax + margin[1]
    offset[0] += xmax + margin[0]
    count += 1
    if count % numofx == 0:
        offset[1] += ymax + margin[1]
        offset[0] = margin[0]

    astr += str

    print('<svg xmlns="http://www.w3.org/2000/svg"', end=' ')
    print('width="{0}" height="{1}">'.format(axmax + margin[0], aymax + margin[1]))
    print(astr)
    print('</svg>')