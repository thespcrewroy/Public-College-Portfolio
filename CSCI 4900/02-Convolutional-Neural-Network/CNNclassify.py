'''Import standard data sceince libraries'''
import os # used for file handling
import sys # used for system specific parameters
from matplotlib import pyplot as plt # used for plotting graphs
import numpy as np # used for numerical computing
from thop import profile # used to compute the number of Multiply-Accumulate Operations (MACs)
import time # used to measure the inference speed

'''Import image processing libraries'''
from PIL import Image # used for image processing

'''Import Torch Libraries'''
import torch # Pytorch
import torchvision # Pytorch Vision
import torchvision.datasets as datasets # import the datasets from torchvision for training and testing
import torchvision.transforms as transforms # used for transforming image
import torch.nn as nn # Pytorch Neural Network class
import torch.optim as optim # Pytorch Optimizer
from torchsummary import summary # used to get the model summary
from resnet20_cifar import resnet20 # import the resnet20 model

'''Import warnings module to ignore warnings'''
import warnings # used to ignore warnings
warnings.filterwarnings('ignore') # ignore warnings

'''
* Random Seeds
    * Manually set the random seed for your model training process
    * Train your network 3 times using different seeds
'''
torch.manual_seed(1234) # so that weights are always initialized to some particular random value

'''
Check if CPU or GPU is available
'''
device = 'cuda' if torch.cuda.is_available() else 'cpu' # set the device to GPU is available else CPU

'''
Specify transforms using torchvision.transforms as transforms
    # One of the methods of normalizing the data from the world of statistics is (x - x.mean()) / x.std()) (F-score)
'''

train_transformations = transforms.Compose([ # compose is used to chain multiple transformations together
    transforms.ToTensor(), # convert the image to a tensor
    transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], std=[0.2023, 0.1994, 0.2010]) # normalize the image
]) # the mean and standard deviation values are the values for the CIFAR-10 dataset

test_transformations = transforms.Compose([ # compose is used to chain multiple transformations together
    transforms.ToTensor(), # convert the image to a tensor
    transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], std=[0.2023, 0.1994, 0.2010]) # Nnrmalize the image
]) # the mean and standard deviation values are the values for the CIFAR-10 dataset

'''----------------- prepare training data -----------------------'''
train_set = datasets.CIFAR10(
    root='./data.cifar10',                          # location of the dataset
    train=True,                                     # this is training data
    transform=train_transformations,                # converts a PIL.Image or numpy.ndarray to torch.FloatTensor of shape (H x W x Channels)
    download=True                                   # if you haven't had the dataset, this will automatically download it for you
) # CIFAR-10 dataset (60,000 32x32 colour images in 10 classes, with 6,000 images per class

'''----------------- prepare testing data -----------------------'''
test_set = datasets.CIFAR10(
    root='./data.cifar10/',                         # location of the dataset
    train=False,                                    # this is testing data
    transform=test_transformations,                 # converts a PIL.Image or numpy.ndarray to torch.FloatTensor of shape (H x W x Channels)
) # CIFAR-10 dataset (60,000 32x32 colour images in 10 classes, with 6,000 images per class

'''
Load datasets into Dataloader using torch library
'''
train_loader = torch.utils.data.DataLoader(train_set, batch_size=64,shuffle=True) # load the training data
test_loader = torch.utils.data.DataLoader(test_set, batch_size =64,shuffle=False) # load the testing data

'''
----------------- Build the Model ------------------------
* Model Specifications
  * Optimization techniques such as mini-batch, batch normilation, dropout, and regularization may be used
  * Requirement: 1st Convolution Layer ONLY
    * Filter (F) = 5x5
    * Stride (S) = 1
    * 0 Padding (P) = 0
    * Number of filters (K) = 32
  * You can add as many convolution layers as you want
    * Keep in mind however, that more convolution layers results in higher training process + larger computation cost
    * Other convolution layers can vary
  * You can add as many fully-connected layers as you want
    * Enhances the model accuracy
    * There is no limitation for the size of the fully connected layers
  * Final testing accuracy should be >= 75% in the end with CIFAR-10 dataset
'''
class CNN(nn.Module):
    def __init__(self):
        super(CNN, self).__init__()

        # First Convolution Layer (As per assignment specification)
        self.conv1 = nn.Conv2d(in_channels=3, out_channels=32, kernel_size=5, stride=1, padding=0)  # 5x5x32 filter Convolution Layer
        self.bn1 = nn.BatchNorm2d(num_features=32)  # 32 Batch Normalization
        self.maxpool1 = nn.MaxPool2d(kernel_size=2, stride=2)  # 2x2 Max Pooling

        # Additional Convolutional Layers (Assignment allows more)
        self.conv2 = nn.Conv2d(in_channels=32, out_channels=64, kernel_size=3, stride=1, padding=1)  # 3x3x64 filter Convolution Layer
        self.bn2 = nn.BatchNorm2d(num_features=64) # 64 Batch Normalization
        
        self.conv3 = nn.Conv2d(in_channels=64, out_channels=128, kernel_size=3, stride=1, padding=1) # 3x3x128 filter Convolution Layer
        self.bn3 = nn.BatchNorm2d(num_features=128) # 128 Batch Normalization
        self.maxpool2 = nn.MaxPool2d(kernel_size=2, stride=2) # 2x2 Max Pooling

        # Fully Connected Layers
        self.fc1 = nn.Linear(in_features=7*7*128, out_features=512)  # 6272 input layer; k = 512 output layer
        # (7x7x128) before flattening --> 6272 (after flattening) --> 512 output neurons

        self.drop1 = nn.Dropout(p=0.3) # Dropout Layer
        self.fc2 = nn.Linear(in_features=512, out_features=256)  # 512 output neurons; k = 256 output layer
        self.drop2 = nn.Dropout(p=0.3) # Dropout Layer
        self.fc3 = nn.Linear(in_features=256, out_features=10)  # 256 output neurons; k = 10 output layer

        # ReLU Activation Function
        self.relu = nn.ReLU() # ReLU Activation Function

    '''
    My Model: 
    1) Conv --> Batch Normalization --> ReLU --> Max
    2) Conv --> Batch Normalization --> ReLUs
    3) Conv --> Batch Normalization --> ReLU --> Max
    4) Flatten
    5) Fully Connected Layer --> ReLU --> Dropout
    6) Fully Connected Layer --> ReLU --> Dropout
    7) Fully Connected Layer
    '''
    def forward(self, x):
        # Conv1 -> BN -> ReLU -> MaxPool
        x = self.conv1(x) # (32 - 5 + 2*0)/1 + 1 = 28x28x32 output layer
        x = self.bn1(x) # 28x28x32
        x = self.relu(x) # ReLU Activation Function
        x = self.maxpool1(x) # (28 - 2)/2 + 1 = 14x14x32 output layer

        # Conv2 -> BN -> ReLU
        x = self.conv2(x) # (14 - 3 + 2*1)/1 + 1 = 14x14x64 output layer
        x = self.bn2(x) # 14x14x64
        x = self.relu(x) # ReLU Activation Function

        # Conv3 -> BN -> ReLU -> MaxPool
        x = self.conv3(x) # (14 - 3 + 2*1)/1 + 1 = 14x14x128 output layer
        x = self.bn3(x) # 14x14x128
        x = self.relu(x) # ReLU Activation Function
        x = self.maxpool2(x) # (14 - 2)/2 + 1 = 7x7x128 output layer

        x = x.view(x.size(0), -1)  # Flatten the output for the Fully Connected Layer

        # FC1 -> ReLU -> Dropout
        x = self.fc1(x) # (7 - 7 + 2*0)/1 + 1 = 1x1x512 output layer
        x = self.relu(x) # ReLU Activation Function
        x = self.drop1(x) # 0.3 Dropout

        # FC2 -> ReLU -> Dropout
        x = self.fc2(x) # (1 - 1 + 2*0)/1 + 1 = 1x1x256 output layer
        x = self.relu(x) # ReLU Activation Function
        x = self.drop2(x) # 0.3 Dropout

        # FC3
        x = self.fc3(x) # (1 - 1 + 2*0)/1 + 1 = 1x1x10 output layer (10 classes)
        return x
    
'''
Instantiate the model, loss function, and optimizer
'''
model = CNN().to(device) # Create an instance of the model and send it to the device
n_epoch = 20 # number of epochs
lr = 0.001 # Learning rate
optimizer = optim.Adam(model.parameters(),lr = lr) # Adam optimizer is used
loss_fn = nn.CrossEntropyLoss() # CrossEntropyLoss is used for multi-class classification as a loss function   

'''
Containers for storing data
'''

train_correct = 0 # number of correct predictions in training set
train_samples = 0 # number of samples in training set
test_correct = 0 # number of correct predictions in testing set
test_samples = 0 # number of samples in testing set

train_losses = [] # list to store training losses
test_losses = [] # list to store testing losses
train_acc_list = [] # list to store training accuracies
test_acc_list = [] # list to store testing accuracies

# ----------------- Main Training Loop -----------------------
def train():
    for epoch in range(n_epoch):  
        model.train()
        running_loss = 0.0
        train_correct = 0
        train_samples = 0

        for X, y in train_loader:
            X, y = X.to(device), y.to(device)  
            optimizer.zero_grad()
            y_pred = model(X)  
            loss = loss_fn(y_pred, y)  
            loss.backward()
            optimizer.step()

            running_loss += loss.item()
            _, predicted = torch.max(y_pred, 1)
            train_correct += (predicted == y).sum().item()
            train_samples += y.size(0)

        model.eval()
        test_loss = 0.0
        test_correct = 0
        test_samples = 0

        with torch.no_grad():
            for images, labels in test_loader:
                images, labels = images.to(device), labels.to(device)  
                outputs = model(images)  
                loss_val = loss_fn(outputs, labels)  
                test_loss += loss_val.item()  
                _, predicted = torch.max(outputs, 1)  
                test_correct += (predicted == labels).sum().item()
                test_samples += labels.size(0)

        # Compute averages
        train_loss = running_loss / len(train_loader)
        test_loss = test_loss / len(test_loader)
        train_accuracy = 100 * train_correct / train_samples
        test_accuracy = 100 * test_correct / test_samples

        # Store results for plotting later
        train_losses.append(train_loss)
        test_losses.append(test_loss)
        train_acc_list.append(train_accuracy)
        test_acc_list.append(test_accuracy)

        print(f"Epoch: {epoch+1} Train Loss: {train_loss:.3f} Train Accuracy: {train_accuracy:.2f}% " +
                f"Test Loss: {test_loss:.3f} Test Accuracy: {test_accuracy:.2f}%")
    
    ep = list(range(n_epoch))  # X-axis

    plt.figure(figsize=(8, 6)) # set figure size
        
    plt.plot(ep, train_acc_list, label='Training Accuracy', color='blue', linestyle='-')  # plot training accuracy
    plt.plot(ep, test_acc_list, label="Test Accuracy", color='red', linestyle='-')

    plt.xlabel("Number of Training Epochs") # set x-axis label
    plt.ylabel("Accuracy") # set y-axis label
    plt.title("Training vs. Test Accuracy Over Epochs") # set title

    plt.legend() # show legend
    plt.grid(True, linestyle="--", alpha=0.5) # show grid
    plt.show() # display the plot

    plt.figure(figsize=(8, 6))  # Set figure size
    
    save_model() # save the model

    '''Report mean and standard deviation of the accuracies'''
    train_mean = np.mean(train_acc_list)
    train_std = np.std(train_acc_list)
    test_mean = np.mean(test_acc_list)
    test_std = np.std(test_acc_list)

    # Print results
    print(f"Train Accuracy Mean: {train_mean:.2f}% | Std Dev: {train_std:.2f}%")
    print(f"Test Accuracy Mean: {test_mean:.2f}% | Std Dev: {test_std:.2f}%")

    ''' Report the computation cost in MACs'''
    compute_macs_and_params(model, "CNN") # compute MACs and number of parameters for my neural network



'''
Save the model function
'''

def save_model():
    """Save the model to the 'model' folder."""
    if not os.path.exists("model"):
        os.makedirs("model")
    torch.save(model.state_dict(), "./model/best_model.pt")

'''
Load the model function
'''
def load_model():
    """Load the model from the 'model' folder."""
    model_path = "./model/best_model.pt"
    if os.path.exists(model_path):
        model.load_state_dict(torch.load(model_path, map_location=device))
        model.eval()
    else:
        print("Saved model not found at", model_path)
        sys.exit(1)

'''
Compute MACs and number of parameters for the given model using THOP'
'''
def compute_macs_and_params(model, model_name="Model"):
    """Compute MACs and number of parameters for the given model using THOP."""
    model.eval()
    dummy_input = torch.randn(1, 3, 32, 32).to(device)

    with torch.no_grad():
        macs, params = profile(model, inputs=(dummy_input,))
    
    print(f"\n{model_name} Computation Cost:")
    print(f"MACs (Multiply-Accumulate Operations): {macs / 1e6:.2f}M")
    print(f"Number of Parameters: {params / 1e6:.2f}M")

# ----------------- Testing Function for a Single Image -----------------------
def test_image(image_path):
    """Load a single image and predict its class."""
    # CIFAR-10 classes dictionary
    classes = {
        0: 'airplane',
        1: 'automobile',
        2: 'bird',
        3: 'cat',
        4: 'deer',
        5: 'dog',
        6: 'frog',
        7: 'horse',
        8: 'ship',
        9: 'truck'
    }

    # Create transformation (resize to 32x32, convert to tensor, normalize)
    transformation_new = transforms.Compose([
        transforms.Resize((32, 32)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.4914, 0.4822, 0.4465],
                             std=[0.2023, 0.1994, 0.2010])
    ])

    load_model()  # Load the saved model

    try:
        image = Image.open(image_path).convert('RGB')
    except Exception as e:
        print("Error opening image:", e)
        sys.exit(1)
    
    image_tensor = transformation_new(image).unsqueeze(0).to(device)
    model.eval()
    with torch.no_grad():
        output = model(image_tensor)
        _, predicted = torch.max(output, 1)
    print(f"prediction result: {classes[predicted.item()]}")

    # ----------------- Feature Map Visualization -----------------
    model.eval() # set model to evaluation mode
    with torch.no_grad(): # turn off gradient tracking
        first_conv_layer = model.conv1  # forward pass through first convolutional layer
        feature_maps = first_conv_layer(image_tensor) # Get the feature maps
    feature_maps = feature_maps.cpu().squeeze(0).detach().numpy()  # move to CPU and detach from graph

    # -------------------- Visualization --------------------
    fig, axes = plt.subplots(4, 8, figsize=(10, 5))  # 4x8 grid (since there are 32 filters)
    for i, ax in enumerate(axes.flat): # iterate through the axes
        if i < feature_maps.shape[0]:  # ensure index is within range
            ax.imshow(feature_maps[i], cmap="gray")  # show each feature map
            ax.axis("off") # Ttrn off axis
        else: # hide unused subplots
            ax.set_visible(False)  # hide unused subplots

    plt.suptitle("Feature Maps of First Convolution Layer", fontsize=12) # set title
    plt.savefig("CONV_rslt.png")  # save the figure
    plt.show() # display the plot

def test_resnet20():
    """Test the pre-trained ResNet-20 model on the CIFAR-10 test set."""
    device = torch.device("cuda" if torch.cuda.is_available() else "cpu")
    model = resnet20().to(device)  # Load ResNet-20
    model_path = "./resnet20_cifar10_pretrained.pt"
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()

    test_correct = 0
    test_samples = 0
    test_loss = 0.0
    loss_fn = nn.CrossEntropyLoss()  # Loss function

    with torch.no_grad():
        for images, labels in test_loader:
            images, labels = images.to(device), labels.to(device)
            outputs = model(images)
            loss_val = loss_fn(outputs, labels)
            test_loss += loss_val.item()
            _, predicted = torch.max(outputs, 1)
            test_correct += (predicted == labels).sum().item()
            test_samples += labels.size(0)

    test_accuracy = 100 * test_correct / test_samples
    print(f"ResNet-20 Test Accuracy: {test_accuracy:.2f}%")

    compute_macs_and_params(model, "ResNet-20")  # compute MACs and number of parameters

def inference_speed_test(model, num_iterations=1000):
    """Measure the inference speed of a model."""
    model.eval()
    dummy_input = torch.randn(1, 3, 32, 32).to(device)

    # Warm-up iterations
    for _ in range(10):
        _ = model(dummy_input)

    # Measure inference time
    start_time = time.time()
    for _ in range(num_iterations):
        _ = model(dummy_input)
    total_time = time.time() - start_time

    avg_time_per_inference = total_time / num_iterations
    print(f"Average Inference Time: {avg_time_per_inference:.6f} seconds")

# ----------------- Main (Command-Line Interface) -----------------------
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python CNNclassify.py [train | test xxx.png | resnet20]")
        sys.exit(1)

    mode = sys.argv[1].lower()

    if mode == "train":
        train()
        
        # Only run inference speed test AFTER training completes
        print("\nMeasuring inference speed for CNN model:")
        inference_speed_test(model)

    elif mode == "test":
        if len(sys.argv) < 3:
            print("Usage: python CNNclassify.py test image_path")
            sys.exit(1)
        test_image(sys.argv[2])

    elif mode == "resnet20":
        test_resnet20()
        
        # Only run inference speed test AFTER ResNet-20 testing completes
        print("\nMeasuring inference speed for ResNet-20:")
        test_resnet = resnet20().to(device)
        test_resnet.load_state_dict(torch.load("./resnet20_cifar10_pretrained.pt", map_location=device))
        inference_speed_test(test_resnet)

    else:
        print("Invalid mode. Use 'train', 'test', or 'resnet20'.")
