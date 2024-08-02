#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Function to determine the number of rows and columns */
void getDimensions(int* rows, int* cols) {
    char line[1024];
    int temp_cols; 

    *rows = 0;
    *cols = 0;
    temp_cols = 0;
    while (fgets(line, sizeof(line), stdin)) {
        if (*cols == 0) {
            char* ptr = line;
            double temp;
            while (sscanf(ptr, "%lf", &temp) == 1) {
                temp_cols++;
                while (*ptr != ',' && *ptr != '\0' && *ptr != '\n') ptr++;
                if (*ptr == ',') ptr++;
            }
            *cols = temp_cols;
        }
        (*rows)++;
    }
    rewind(stdin);
}

/* Function to read a 2D array from standard input */
double** readFileTo2DArray(int* rows, int* cols, int k) {
    int i, j, l;
    double** array;

    getDimensions(rows, cols);
    if (*rows == 0 || *cols == 0) {
        printf("An Error Has Occurred");
        exit(1);
    }
    if (*rows <= k){
        printf("Invalid number of clusters!\n");
        exit(1);
    }
    array = (double**)malloc(*rows * sizeof(double*));
    if (array == NULL) {
        perror("An Error Has Occurred");
        exit(1);
    }

    for (i = 0; i < *rows; i++) {
        array[i] = (double*)malloc(*cols * sizeof(double));
        if (array[i] == NULL) {
            perror("An Error Has Occurred");
            for (l = 0; l < i; l++) {
                free(array[l]);
            }
            free(array);
            exit(1);
        }
    }

    for (i = 0; i < *rows; i++) {
        for (j = 0; j < *cols; j++) {
            if (scanf("%lf,", &array[i][j]) != 1) {
                perror("An Error Has Occurred");
                for (l = 0; l < *rows; l++) {
                    free(array[l]);
                }
                free(array);
                exit(1);
            }
        }
    }

    return array;
}


/* Function to free the 2D array */
void free2DArray(double** array, int rows) {
    int i;
    for (i = 0; i < rows; i++) {
        free(array[i]);
    }
    free(array);
}

/* Function to calculate the Euclidean distance between two arrays of doubles */
double Euclidian_dis(double* lst1, double* lst2, int dimensions) {
    double sum;
    int i;
    sum = 0.0;
    for (i = 0; i < dimensions; i++) {
        double diff = lst1[i] - lst2[i];
        sum += diff * diff;
    }
    return sqrt(sum);
}

/* Custom round function */
double custom_round(double value) {
    return floor(value + 0.5);
}

/* Function to build clusters based on the nearest centroids */
void cluster_builder(double** centroids, int num_centroids, double** df, int num_points, double*** clusters, int* cluster_sizes, int dimensions) {
    int i, j;
    for (i = 0; i < num_points; i++) {
        double* lst = df[i];
        int min_index = 0;
        double min_value = Euclidian_dis(centroids[0], lst, dimensions);
        for (j = 1; j < num_centroids; j++) {
            double tmp = Euclidian_dis(centroids[j], lst, dimensions);
            if (tmp < min_value) {
                min_value = tmp;
                min_index = j;
            }
        }
        clusters[min_index][cluster_sizes[min_index]++] = lst;
    }
}

/* Function to update centroids based on clusters */
int centroid_setter(double** centroids, double*** clusters, int* cluster_sizes, int num_centroids, int dimensions) {
    int i, j, k, cnt = 0;
    for (i = 0; i < num_centroids; i++) {
        double* ans = (double*)malloc(dimensions * sizeof(double));
        if (ans == NULL) {
            perror("An Error Has Occurred");
            exit(1);
        }
        for (k = 0; k < dimensions; k++) {
            double sums = 0.0;
            for (j = 0; j < cluster_sizes[i]; j++) {
                sums += clusters[i][j][k];
            }
            ans[k] = custom_round(sums / cluster_sizes[i] * 10000) / 10000.0;
        }
        if (Euclidian_dis(centroids[i], ans, dimensions) <= 0.001) {
            cnt++;
        } else {
            for (k = 0; k < dimensions; k++) {
                centroids[i][k] = ans[k];
            }
        }
        free(ans);
    }
    return cnt == num_centroids;
}

/* K-Means function */
void k_means(int k, int max_iter) {
    int rows, cols, i, j, t;
    double** df = readFileTo2DArray(&rows, &cols, k);
    double** centroids;
    double*** clusters;
    int* cluster_sizes;

    if (df == NULL) {
        printf("An Error Has Occurred\n");
        exit(1);
    }

    centroids = (double**)malloc(k * sizeof(double*));
    if (centroids == NULL) {
        perror("An Error Has Occurred");
        exit(1);
    }
    for (i = 0; i < k; i++) {
        centroids[i] = (double*)malloc(cols * sizeof(double));
        if (centroids[i] == NULL) {
            perror("An Error Has Occurred");
            for (j = 0; j < i; j++) {
                free(centroids[j]);
            }
            free(centroids);
            exit(1);
        }
        for (j = 0; j < cols; j++) {
            centroids[i][j] = df[i][j];
        }
    }

    clusters = (double***)malloc(k * sizeof(double**));
    if (clusters == NULL) {
        perror("An Error Has Occurred");
        for (i = 0; i < k; i++) {
            free(centroids[i]);
        }
        free(centroids);
        exit(1);
    }
    for (i = 0; i < k; i++) {
        clusters[i] = (double**)malloc(rows * sizeof(double*));
        if (clusters[i] == NULL) {
            perror("An Error Has Occurred");
            for (j = 0; j < i; j++) {
                free(clusters[j]);
            }
            for (j = 0; j < k; j++) {
                free(centroids[j]);
            }
            free(centroids);
            free(clusters);
            exit(1);
        }
    }
    cluster_sizes = (int*)calloc(k, sizeof(int));
    if (cluster_sizes == NULL) {
        perror("An Error Has Occurred");
        for (i = 0; i < k; i++) {
            free(clusters[i]);
            free(centroids[i]);
        }
        free(centroids);
        free(clusters);
        exit(1);
    }
    t = 0;
    while (t < max_iter) {
        for (i = 0; i < k; i++) {
            cluster_sizes[i] = 0;
        }
        cluster_builder(centroids, k, df, rows, clusters, cluster_sizes, cols);
        if (centroid_setter(centroids, clusters, cluster_sizes, k, cols)) {
            break;
        }
        t++;
    }
    for (i = 0; i < k; i++) {
        for (j = 0; j < cols; j++) {
            if (j != 0){
                printf(",");
            }
            printf("%.4f", centroids[i][j]);
        }
        printf("\n");
    }
    free2DArray(df, rows);
    free2DArray(centroids, k);
    for (i = 0; i < k; i++) {
        free(clusters[i]);
    }
    free(clusters);
    free(cluster_sizes);
}

int main(int argc, char* argv[]) {
    int k, max_iter;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <number_of_clusters> <max_iterations>\n", argv[0]);
        return 1;
    }

    k = atoi(argv[1]);
    if (k <= 1) {
        fprintf(stderr, "Invalid number of clusters!\n");
        return 1;
    }
    if (argc == 2){
        max_iter = 200;
    }
    else {max_iter = atoi(argv[2]);}
    if (max_iter <= 1 || max_iter >= 1000) {
        fprintf(stderr, "Invalid maximum iteration!\n");
        return 1;
    }

    k_means(k, max_iter);
    return 0;
}
