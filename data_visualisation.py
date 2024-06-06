import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def create_bar_chart_from_csv(csv_filename):
    # Read the CSV file into a DataFrame
    df = pd.read_csv(csv_filename, header=None, names=['Order Type', 'Times', 'Executed Orders', 'AVL Tree Balances'])
    
    # Ensure the CSV has the necessary columns
    if 'Times' not in df.columns or 'Order Type' not in df.columns:
        raise ValueError("CSV file must contain 'orderType' and 'time' columns")
    
    # Extract data for the bar chart
    times = df['Times']
    orderTypes = df['Order Type']
    average_time = times.mean()
    print(f"Average time: {average_time}")

    # Print the 20 rows with the highest times
    # top_20_times = df.nlargest(20, 'Times')
    # print("\nTop 20 rows with the highest times:")
    # print(top_20_times)

    # Create a pie chart of the different order types
    order_type_counts = orderTypes.value_counts()
    order_type_counts = order_type_counts[::-1]
    plt.figure(figsize=(8, 8))
    plt.pie(order_type_counts, labels=order_type_counts.index, autopct='%1.1f%%', startangle=0, colors=plt.cm.tab20.colors)
    plt.title('Distribution of Order Types')
    plt.show()

    # Create a histogram of the times
    filtered_times = times[times <= 5000]
    plt.figure(figsize=(10, 6))
    plt.hist(filtered_times, bins=50, color='skyblue', edgecolor='black')
    plt.title('Histogram of Times')
    plt.xlabel('Time')
    plt.ylabel('Frequency')
    plt.show()
    
    # Exclude 'Market' and 'AddMarketLimit' order types
    excluded_order_types = ['Market', 'AddMarketLimit']
    filtered_df = df[~df['Order Type'].isin(excluded_order_types)]

    # Calculate mean, 25th, and 75th percentiles for each order type
    stats = filtered_df.groupby('Order Type')['Times'].agg(['mean', lambda x: x.quantile(0.25), lambda x: x.quantile(0.85)]).reset_index()
    stats.columns = ['Order Type', 'mean', '25th', '75th']
    stats['error_lower'] = stats['mean'] - stats['25th']
    stats['error_upper'] = stats['75th'] - stats['mean']
    stats = stats.sort_values(by='mean')

    # Create a bar chart with error bars
    plt.figure(figsize=(12, 6))
    plt.bar(stats['Order Type'], stats['mean'], yerr=[stats['error_lower'], stats['error_upper']], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Mean Processing Time for Each Order Type with 25th and 75th Percentile Error Bars')
    plt.xlabel('Order Type')
    plt.ylabel('Mean Processing Time')
    plt.xticks(rotation=45)
    plt.show()

    # Filter for Market and AddMarketLimit order types
    market_df = df[df['Order Type'].isin(['Market', 'AddMarketLimit'])]
    # market_df = market_df[market_df['Executed Orders'] <= 15]

    # Calculate mean, 25th, and 75th percentiles for each count number
    stats = market_df.groupby('Executed Orders')['Times'].agg(['mean']).reset_index()
    stats.columns = ['Executed Orders', 'mean']

    # Plot the bar chart for average time for each different count
    plt.figure(figsize=(12, 6))
    plt.bar(stats['Executed Orders'], stats['mean'], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Average Processing Time for Market and AddMarketLimit by Executed Orders')
    plt.xlabel('Executed Orders')
    plt.ylabel('Average Time')
    plt.show()


    balance_df = df[df['AVL Tree Balances'] != 0]

    # Calculate mean, 25th, and 75th percentiles for each count number
    stats = balance_df.groupby('AVL Tree Balances')['Times'].agg(['mean', 'count']).reset_index()
    stats.columns = ['AVL Tree Balances', 'mean', 'count']
    stats_filtered = stats[stats['count'] >= 5]

    # Plot the bar chart for average time for each different count
    plt.figure(figsize=(12, 6))
    plt.bar(stats_filtered['AVL Tree Balances'], stats_filtered['mean'], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Average Processing Time for Market and AddMarketLimit by Executed Orders')
    plt.xlabel('Executed Orders')
    plt.ylabel('Average Time')
    plt.show()

    # Create figure and 3D axis
    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(111, projection='3d')
    
    # Calculate mean for each count number
    stats = market_df.groupby(['Executed Orders', 'AVL Tree Balances'])['Times'].agg(['mean', 'count']).reset_index()
    stats.columns = ['Executed Orders', 'AVL Tree Balances', 'mean', 'count']
    stats_filtered = stats[stats['count'] >= 5]
    
    x = stats_filtered['Executed Orders']
    y = stats_filtered['AVL Tree Balances']
    z = stats_filtered['mean']
    x_flipped = x.max() - x

    ax.bar3d(x_flipped, y, 0, 1, 1, z, color='skyblue')
    ax.set_xlabel('Executed Orders')
    ax.set_ylabel('AVL Tree Balances')
    ax.set_zlabel('Time')
    # ax.set_xticks([0, 20, 40, 60, 80, 100, 120, 140])
    # ax.set_xticklabels([140, 120, 100, 80, 60, 40, 20, 0])
    plt.title('3D Bar Chart of Time with Executed Orders and AVL Tree Balances')
    plt.show()


csv_filename = './build/order_processing_times.csv'
create_bar_chart_from_csv(csv_filename)
