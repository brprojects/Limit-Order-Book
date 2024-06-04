import pandas as pd
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D

def create_bar_chart_from_csv(csv_filename):
    # Read the CSV file into a DataFrame
    df = pd.read_csv(csv_filename, header=None, names=['Order Type', 'Times', 'Count', 'Count 2', 'Count 3'])
    
    # Ensure the CSV has the necessary columns
    if 'Times' not in df.columns or 'Order Type' not in df.columns:
        raise ValueError("CSV file must contain 'orderType' and 'time' columns")
    
    # Extract data for the bar chart
    times = df['Times']
    orderTypes = df['Order Type']
    average_time = times.mean()
    print(average_time)

    # Print the 20 rows with the highest times
    top_20_times = df.nlargest(20, 'Times')
    print("\nTop 20 rows with the highest times:")
    print(top_20_times)

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
    stats = filtered_df.groupby('Order Type')['Times'].agg(['mean', lambda x: x.quantile(0.25), lambda x: x.quantile(0.95)]).reset_index()
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
    # market_df = df[df['Order Type'].isin(['Market', 'AddMarketLimit']) & df['Count 2'] == 0]
    market_df = df[df['Order Type'].isin(['Market', 'AddMarketLimit'])]
    # market_df = market_df[market_df['Count'] <= 15]

    # Calculate mean, 25th, and 75th percentiles for each count number
    stats = market_df.groupby('Count')['Times'].agg(['mean', lambda x: x.quantile(0.25), lambda x: x.quantile(0.95)]).reset_index()
    stats.columns = ['Count', 'mean', '25th', '75th']
    stats['error_lower'] = stats['mean'] - stats['25th']
    stats['error_upper'] = stats['75th'] - stats['mean']

    # Plot the bar chart for average time for each different count
    plt.figure(figsize=(12, 6))
    plt.bar(stats['Count'], stats['mean'], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Average Processing Time for Market and AddMarketLimit by Count')
    plt.xlabel('Count')
    plt.ylabel('Average Time')
    plt.show()


    balance_df = df[df['Count 3'] != 0]

    # Calculate mean, 25th, and 75th percentiles for each count number
    stats = balance_df.groupby('Count 3')['Times'].agg(['mean', lambda x: x.quantile(0.25), lambda x: x.quantile(0.95)]).reset_index()
    stats.columns = ['Count 3', 'mean', '25th', '75th']
    stats['error_lower'] = stats['mean'] - stats['25th']
    stats['error_upper'] = stats['75th'] - stats['mean']

    # Plot the bar chart for average time for each different count
    plt.figure(figsize=(12, 6))
    plt.bar(stats['Count 3'], stats['mean'], capsize=5, color='skyblue', edgecolor='black')
    plt.title('Average Processing Time for Market and AddMarketLimit by Count')
    plt.xlabel('Count')
    plt.ylabel('Average Time')
    plt.show()

    # Create figure and 3D axis
    fig = plt.figure(figsize=(10, 6))
    ax = fig.add_subplot(111, projection='3d')
    
    # Calculate mean, 25th, and 75th percentiles for each count number
    stats = market_df.groupby(['Count', 'Count 3'])['Times'].agg(['mean', lambda x: x.quantile(0.25), lambda x: x.quantile(0.95)]).reset_index()
    stats.columns = ['Count', 'Count 3', 'mean', '25th', '75th']
    stats['error_lower'] = stats['mean'] - stats['25th']
    stats['error_upper'] = stats['75th'] - stats['mean']
    x = stats['Count']
    y = stats['Count 3']
    z = stats['mean']

    ax.bar3d(x, y, 0, 1, 1, z, color='skyblue')
    ax.set_xlabel('Count')
    ax.set_ylabel('Count 3')
    ax.set_zlabel('Time')
    plt.title('3D Bar Chart of Time with Count and Count 2')
    plt.show()


csv_filename = './build/order_processing_times.csv'
create_bar_chart_from_csv(csv_filename)
