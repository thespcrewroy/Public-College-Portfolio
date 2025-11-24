# CRAG Implementation - Project Summary

## Overview

This project implements the clustering component of **CRAG (Clustered Retrieved Augmented Generation)**, a research paper published in May 2024. The implementation demonstrates how K-Means clustering can reduce data volume in text review datasets while maintaining semantic information.

## What Was Fixed

### Original Issues

1. **Incorrect data structure** - Code referenced non-existent columns (`asin`, `items`, `reviews`)
2. **Missing preprocessing** - No filtering for products with sufficient reviews
3. **Lack of comments** - Code was difficult to understand
4. **No data exploration** - Missing analysis of review distribution
5. **Incomplete evaluation** - Limited visualization and metrics

### Improvements Made

#### 1. **Corrected Data Loading** (Cell 2)

- Uses actual CSV columns: `Product Name`, `Rating`, `Reviews`
- Added data exploration and statistics
- Shows dataset structure and missing values

#### 2. **Added Preprocessing Pipeline** (Cells 3-4)

- **Cell 3:** Clean data and analyze review distribution per product
- **Cell 4:** Filter products with sufficient reviews (min 20 reviews)
  - Ensures meaningful clustering (need ≥ k=4 reviews)
  - Focuses on products with enough data
  - **Visualizes distribution** of reviews per product
  - Shows top products for clustering

#### 3. **Enhanced Embedding Generation** (Cell 5)

- Added detailed comments explaining the transformation
- Uses same model as CRAG paper: `paraphrase-multilingual-mpnet-base-v2`
- Includes progress tracking and memory statistics
- Normalizes embeddings for better distance calculations

#### 4. **Improved Clustering** (Cell 6)

- Added progress bar for user feedback
- Tracks successfully clustered vs skipped products
- Uses k=4 as specified in CRAG paper
- Clear comments explaining each step

#### 5. **Comprehensive Evaluation** (Cell 7)

- **Silhouette Score:** Measures clustering quality
  - Interpretation guide (strong/moderate/weak)
  - Statistics (mean, min, max, std dev)
- **Data Reduction Calculation:**
  - Shows exact reduction percentage
  - Compares to CRAG paper results (46-90%)
  - Calculates compression ratio

#### 6. **Visualizations** (Cell 8)

- Distribution of reviews per product
- Distribution of cluster sizes
- Mean values highlighted with vertical lines

#### 7. **Representative Selection** (Cell 9)

- Finds centroid for each cluster
- Selects review closest to centroid
- Shows compression statistics

#### 8. **Output Files** (Cell 10)

- Saves 3 files to `outputs/` folder:
  1. `clustered_reviews.csv` - All reviews with cluster IDs
  2. `cluster_representatives.csv` - One review per cluster (for RAG)
  3. `clustering_summary.txt` - Statistics and metrics

#### 9. **Optional Analysis** (Cells 11-13)

- **Cell 11:** Deep dive into one product's clustering
- **Cell 12:** Compare K-Means vs Hierarchical Clustering
- **Cell 13:** Elbow Method to validate k=4 choice

## How to Run

### Prerequisites

```bash
pip install numpy pandas scikit-learn matplotlib sentence-transformers tqdm
```

### Execution Steps

1. **Ensure `Amazon_Unlocked_Mobile.csv` is in the project folder**
2. **Run cells sequentially** (top to bottom)
3. **First run will download the model** (~420MB, one-time only)
4. **Adjust `min_reviews_per_product`** in Cell 4 based on your machine:
   - Fast machines: 20-50 reviews
   - Slower machines: 10-20 reviews
   - Very limited: 5-10 reviews

### Expected Runtime

- **Loading data:** < 1 minute
- **Generating embeddings:** 5-15 minutes (depends on dataset size)
- **Clustering:** 2-5 minutes
- **Evaluation & output:** < 1 minute

## Key Parameters to Adjust

### Cell 4: `min_reviews_per_product`

```python
min_reviews_per_product = 20  # Lower this if too slow
```

- **Higher value** = fewer products but better clustering quality
- **Lower value** = more products but may have weak clusters

### Cell 5: `batch_size`

```python
batch_size=64  # Lower if memory issues
```

- Reduce to 32 or 16 if you get memory errors

### Cell 6: `k` (number of clusters)

```python
k = 4  # CRAG paper uses 4
```

- Use Cell 13 (Elbow Method) to test if 4 is optimal for your data

## Expected Results

### Metrics

- **Silhouette Score:** 0.25 - 0.60 (depends on data quality)
  - > 0.5 = Strong clustering
  - 0.25-0.5 = Moderate clustering
  - < 0.25 = Weak clustering
- **Data Reduction:** 40-80% typical
  - CRAG paper achieved 46-90%
  - Your result depends on product review counts

### Output Files

All saved in `outputs/` folder:

- `clustered_reviews.csv` - Full dataset with cluster labels
- `cluster_representatives.csv` - Reduced dataset for RAG systems
- `clustering_summary.txt` - Report with all metrics

## Project Structure

```
03-CRAGFinalProject/
├── Amazon_Unlocked_Mobile.csv    # Your dataset
├── project.ipynb                 # Main implementation
├── README.md                     # This file
└── outputs/                      # Generated files
    ├── clustered_reviews.csv
    ├── cluster_representatives.csv
    └── clustering_summary.txt
```

## Presentation Notes

### Part 1: Research Paper (7 minutes)

- CRAG overview and motivation
- Why clustering for RAG systems?
- Original methodology and results
- Key contribution: Data reduction while preserving quality

### Part 2: Implementation (8 minutes)

1. **Dataset & Preprocessing** (2 min)
   - Show Cell 2-4 outputs
   - Explain filtering strategy
2. **Embeddings & Clustering** (2 min)
   - Show Cell 5-6 outputs
   - Explain semantic similarity concept
3. **Results & Evaluation** (3 min)
   - Show Cell 7-8 visualizations
   - Present silhouette scores
   - **Highlight data reduction percentage**
4. **Comparison** (1 min)
   - Show Cell 12 (K-Means vs Hierarchical)
   - Discuss trade-offs

## Troubleshooting

### "Memory Error" during embedding generation

- Reduce `batch_size` in Cell 5 to 32 or 16
- Reduce `min_reviews_per_product` in Cell 4 to 10

### "Model download fails"

- Check internet connection
- Model will be cached after first download in `~/.cache/huggingface/`

### "Clustering takes too long"

- Reduce `min_reviews_per_product` to process fewer products
- Consider setting `max_products` limit in Cell 4 (see commented code)

### "Silhouette score is very low"

- Normal for some products with similar reviews
- Try different products (Cell 11 for deep dive)
- Consider increasing `min_reviews_per_product` for better quality

## Extensions for Extra Credit

1. **Add DBSCAN clustering** comparison
2. **Test different embedding models** (all-MiniLM-L6-v2 is faster)
3. **Implement the Elbow Method** across multiple products
4. **Create interactive visualizations** with Plotly
5. **Add sentiment analysis** to cluster labels

## References

- **CRAG Paper:** https://arxiv.org/html/2406.00029v1
- **Dataset:** Amazon Unlocked Mobile Reviews (Kaggle)
- **Model:** sentence-transformers/paraphrase-multilingual-mpnet-base-v2
- **Libraries:** scikit-learn, sentence-transformers, pandas

---

**Good luck with your presentation! 🚀**
