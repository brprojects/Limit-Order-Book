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
    trades_count = df['Executed Orders'].sum()
    balances_count = df['AVL Tree Balances'].sum()
    # print(f'{trades_count}  {balances_count}')
    # print(f"Average time: {average_time}")

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
    # plt.show()
    # plt.savefig('../figures/OrderTypes.png')

    # Create a histogram of the order latencies
    filtered_times = times[times <= 4000]
    plt.figure(figsize=(10, 6))
    plt.hist(filtered_times, bins=40, color='skyblue', edgecolor='black')
    plt.title(f'Orders by Latency Histogram - mean={average_time:.1f}ns ({1000000000/average_time:,.0f} orders/s)')
    plt.xlabel('Latency (ns)')
    plt.ylabel('Number of Orders')
    # plt.show()
    # plt.savefig('../figures/LatencyHistogram.png')
    
    # Exclude 'Market' and 'AddMarketLimit' order types
    excluded_order_types = ['Market', 'AddMarketLimit']
    filtered_df = df[~df['Order Type'].isin(excluded_order_types)]

    # Calculate mean, 15th, and 85th percentiles for each order type
    stats = filtered_df.groupby('Order Type')['Times'].agg(['mean', lambda x: x.quantile(0.15), lambda x: x.quantile(0.85)]).reset_index()
    stats.columns = ['Order Type', 'mean', '25th', '75th']
    stats['error_lower'] = stats['mean'] - stats['25th']
    stats['error_upper'] = stats['75th'] - stats['mean']
    stats = stats.sort_values(by='mean')

    # Create a bar chart with error bars for latency for each order type
    plt.figure(figsize=(12, 6))
    plt.bar(stats['Order Type'], stats['mean'], yerr=[stats['error_lower'], stats['error_upper']], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Latency by Order Type')
    plt.xlabel('Order Type')
    plt.ylabel('Latency (ns)')
    plt.xticks(rotation=45)
    # plt.show()
    # plt.savefig('../figures/OrderTypeLatencies.png', bbox_inches='tight')

    # Filter for Market and AddMarketLimit order types
    market_df = df[df['Order Type'].isin(['Market', 'AddMarketLimit'])]

    # Calculate mean for each number of executed orders
    stats = market_df.groupby('Executed Orders')['Times'].agg(['mean', 'count']).reset_index()
    stats.columns = ['Executed Orders', 'mean', 'count']
    stats_filtered = stats[stats['count'] >= 5]

    # Create a bar chart for latency for number of executed orders
    plt.figure(figsize=(12, 6))
    plt.bar(stats_filtered['Executed Orders'], stats_filtered['mean'], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Latency by Number of Trades')
    plt.xlabel('Number of Trades per Order')
    plt.ylabel('Latency (ns)')
    # plt.show()
    # plt.savefig('../figures/ExecutedOrders.png')


    balance_df = df[df['AVL Tree Balances'] != 0]

    # Calculate mean for each number of AVL tree balances
    stats = balance_df.groupby('AVL Tree Balances')['Times'].agg(['mean', 'count']).reset_index()
    stats.columns = ['AVL Tree Balances', 'mean', 'count']
    stats_filtered = stats[stats['count'] >= 5]

    # Plot the bar chart for latency for number of AVL tree balances
    plt.figure(figsize=(12, 6))
    plt.bar(stats_filtered['AVL Tree Balances'], stats_filtered['mean'], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Latency by Number of AVL Tree Balances')
    plt.xlabel('Number of AVL Tree Balances')
    plt.ylabel('Latency (ns)')
    # plt.show()
    # plt.savefig('../figures/AVLTreeBalances.png')

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

    # Create 3D bar chart for latency for number of orders executed and avl tree balances
    ax.bar3d(x_flipped, y, 0, 1, 1, z, color='skyblue')
    ax.set_xlabel('Number of Trades per Order')
    ax.set_ylabel('AVL Tree Balances')
    ax.set_zlabel('Time (ns)')
    ax.set_xticks([3, 13, 23, 33, 43, 53, 63, 73])
    ax.set_xticklabels([70, 60, 50, 40, 30, 20, 10, 0])
    plt.title('Latency by Number of Trades and AVL Tree Balances')
    # plt.show()
    # plt.savefig('../figures/3D.png')


csv_filename = './order_processing_times.csv'
create_bar_chart_from_csv(csv_filename)
