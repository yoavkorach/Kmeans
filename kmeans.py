import sys

def k_means(k, path, max_iter=200):
    df = text_opener(path)
    assert (k <= len(df))
    t = 0
    centroids = [df[i] for i in range(k)]
    while t < max_iter:
        clusters = [[] for j in range(k)]
        cluster_builder(centroids, df, clusters)
        converge = centroid_setter(centroids, clusters)
        if converge:
            break
        t += 1
    fin = ""
    for i in range(len(centroids)):
        for j in range(len(centroids[i])):
            fin += str(centroids[i][j])
            if j != len(centroids[i]) - 1:
                fin += ","
        if i != len(centroids)-1:
            fin += "\n"
    print(fin)

def centroid_setter(centroids, clusters):
    cnt = 0
    for i in range(len(clusters)):
        ans = []
        ind = 0
        while ind < len(clusters[i][0]):
            sums = 0
            for j in range(len(clusters[i])):
                sums += clusters[i][j][ind]
            ans.append(round(sums / len(clusters[i]), 4))
            ind += 1
        if d(centroids[i], ans) <= 0.001:
            cnt += 1
        else:
            centroids[i] = ans
    return cnt == len(centroids)

def cluster_builder(centroids, df, clusters):
    for i in range(len(df)):
        lst = df[i]
        min_index = 0
        min_value = d(centroids[0], lst)
        for j in range(1, len(centroids)):
            tmp = d(centroids[j], lst)
            if tmp < min_value:
                min_value = tmp
                min_index = j
        clusters[min_index].append(lst)

def d(lst1, lst2):
    k = 0
    if len(lst1) != len(lst2):
        return
    for i in range(len(lst1)):
        k += ((lst1[i] - lst2[i])**2)
    return k ** 0.5

def text_opener(path):
    file = open(path, "r")
    df = []
    for line in file.readlines():
        tmp = line.strip().split(",")
        for i in range(len(tmp)):
            tmp[i] = float(tmp[i])
        df.append(tmp)
    file.close()
    return df

def main():
    if len(sys.argv) < 3:
        print("Usage: python kmeans.py <number_of_clusters> <max_iterations> <input_file>")
        sys.exit(1)
    if len(sys.argv) == 3:
        k = int(sys.argv[1])
        max_iter = 200
        filename = sys.argv[2]
    else:
        k = int(sys.argv[1])
        max_iter = int(sys.argv[2])
        filename = sys.argv[3]
    try:
        assert isinstance(k, int) and 1<k
    except AssertionError:
        print("Invalid number of clusters!")
        sys.exit(1)
    try:
        assert isinstance(max_iter, int) and max_iter>1 and max_iter<1000
    except AssertionError:
        print("Invalid maximum iteration!")
        sys.exit(1) 
    try:
        k_means(k, filename, max_iter)
    except AssertionError:
        print("Invalid number of clusters!")
        sys.exit(1) 
    except:
        print("An Error Has Occurred")
        sys.exit(1)

if __name__ == "__main__":
    main()
