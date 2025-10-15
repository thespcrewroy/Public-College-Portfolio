'''Import standard data sceince libraries'''
import os # used for file handling
import sys # used for system specific parameters
from matplotlib import pyplot as plt # used for plotting graphs
import numpy as np # used for numerical computing

'''Import image processing libraries'''
from PIL import Image # used for image processing

'''Import Torch Libraries'''
import torch # Pytorch
import torchvision.datasets as datasets # Import the datasets from torchvision for training and testing
import torchvision.transforms as transforms # used for transforming image
import torch.nn as nn # Pytorch Neural Network class
import torch.optim as optim # Pytorch Optimizer

'''Import warnings module to ignore warnings'''
import warnings
warnings.filterwarnings('ignore')

'''Set manual seed for the torch randomly generated parameters to have same values'''
torch.manual_seed(42) # So that weights are always initialized to some particular random value

'''Check if CPU or GPU is available'''
device = 'cuda' if torch.cuda.is_available() else 'cpu' # set the device to GPU is available else CPU

'''
Specify transforms using torchvision.transforms as transforms
# One of the methods of normalizing the data from the world of statistics is (x - x.mean()) / x.std()) (F-score)
'''

train_transformations = transforms.Compose([ # Compose is used to chain multiple transformations together
    transforms.ToTensor(), # Convert the image to a tensor
    transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], std=[0.2023, 0.1994, 0.2010]) # Normalize the image
]) # The mean and standard deviation values are the values for the CIFAR-10 dataset

test_transformations = transforms.Compose([ # Compose is used to chain multiple transformations together
    transforms.ToTensor(), # Convert the image to a tensor
    transforms.Normalize(mean=[0.4914, 0.4822, 0.4465], std=[0.2023, 0.1994, 0.2010]) # Normalize the image
]) # The mean and standard deviation values are the values for the CIFAR-10 dataset

'''----------------- prepare training data -----------------------'''
train_set = datasets.CIFAR10(
    root='./data.cifar10',                          # location of the dataset
    train=True,                                     # this is training data
    transform=train_transformations,                # Converts a PIL.Image or numpy.ndarray to torch.FloatTensor of shape (C x H x W)
    download=True                                   # if you haven't had the dataset, this will automatically download it for you
) # CIFAR-10 dataset (60,000 32x32 colour images in 10 classes, with 6,000 images per class

'''----------------- prepare testing data -----------------------'''
test_set = datasets.CIFAR10(
    root='./data.cifar10/',                         # location of the dataset
    train=False,                                    # this is testing data
    transform=test_transformations,                 # Converts a PIL.Image or numpy.ndarray to torch.FloatTensor of shape (C x H x W)
) # CIFAR-10 dataset (60,000 32x32 colour images in 10 classes, with 6,000 images per class

'''Load datasets into Dataloader using torch library'''
train_loader = torch.utils.data.DataLoader(train_set, batch_size=64,shuffle=True) # Load the training data
test_loader = torch.utils.data.DataLoader(test_set, batch_size =64,shuffle=False) # Load the testing data

'''----------------- build the model ------------------------'''
class SwapNet(nn.Module):
    def __init__(self):
        super(SwapNet, self).__init__()
        self.fc1 = nn.Linear(3 * 32 * 32, 512) # 3*32*32 = 3072 (flattened image)
        self.fc2 = nn.Linear(512, 256) # 512 input features, 256 output features
        self.fc3 = nn.Linear(256, 10) # 256 input features, 10 output features (for 10 classes)
        self.drop = nn.Dropout(0.2) # Dropout layer with 20% probability
        self.relu = nn.ReLU() # ReLU activation function

    def forward(self, x):
        x = x.view(x.size(0), -1) # Flatten the CIFAR-10 image (3x32x32 -> 3072)
        
        # First fully connected layer + ReLU + dropout
        x = self.fc1(x) # Fully connected
        x = self.relu(x) # ReLU
        x = self.drop(x) # Dropout
        
        # Second fully connected layer + ReLU + dropout
        x = self.fc2(x) # Fully connected
        x = self.relu(x) # ReLU
        x = self.drop(x) # Dropout
        
        # Output layer (logits for 10 classes; no activation here because CrossEntropyLoss applies softmax internally)
        x = self.fc3(x) # Fully connected
        return x

# Instantiate the model, loss function, and optimizer
model = SwapNet().to(device) # Create an instance of the model and send it to the device (GPU or CPU)
n_epoch = 30 # Number of epochs
lr = 0.001 # Learning rate
optimizer = optim.Adam(model.parameters(),lr = lr) # Adam optimizer is used
loss_fn = nn.CrossEntropyLoss() # CrossEntropyLoss is used for multi-class classification as a loss function  

'''Containers for storing data'''
train_correct = 0 # Number of correct predictions in training set
train_samples = 0 # Number of samples in training set
test_correct = 0 # Number of correct predictions in testing set
test_samples = 0 # Number of samples in testing set

train_losses = [] # List to store training losses
test_losses = [] # List to store testing losses

# ----------------- Main Training Loop -----------------------
def train():
    for epoch in range(n_epoch): # Loop over the dataset multiple times
        model.train() # Set the model to training mode
        running_loss = 0.0 # Initialize running loss
        train_correct = 0 # Initialize number of correct predictions in training set
        train_samples = 0 # Initialize number of samples in training set
        
        for X, y in train_loader: # Loop over the training set
            X, y = X.to(device), y.to(device) # Send data to the device (GPU or CPU)
            optimizer.zero_grad() # Zero the parameter gradients
            y_pred = model(X) # Forward pass
            loss = loss_fn(y_pred, y) # Compute the loss
            loss.backward() # Backward pass
            optimizer.step() # Optimize the model
            
            running_loss += loss.item() # Accumulate the loss
            _, predicted = torch.max(y_pred, 1) # Get the class index with the highest probability
            train_correct += (predicted == y).sum().item() # Count the correct predictions
            train_samples += y.size(0) # Count the samples
        
        # Evaluate on test set
        model.eval() # Set the model to evaluation mode
        test_loss = 0.0 # Initialize test loss
        test_correct = 0 # Initialize number of correct predictions in testing set
        test_samples = 0 # Initialize number of samples in testing set
        with torch.no_grad(): # Disable gradient tracking
            for images, labels in test_loader: # Loop over the testing set
                images, labels = images.to(device), labels.to(device) # Send data to the device (GPU or CPU)
                outputs = model(images) # Forward pass
                loss_val = loss_fn(outputs, labels) # Compute the loss
                test_loss += loss_val.item() # Accumulate the loss
                _, predicted = torch.max(outputs, 1) # Get the class index with the highest probability
                test_correct += (predicted == labels).sum().item() # Count the correct predictions
                test_samples += labels.size(0) # Count the samples
        
        train_loss = running_loss / len(train_loader) # Average training loss
        test_loss = test_loss / len(test_loader) # Average testing loss
        train_accuracy = 100 * train_correct / train_samples # Training accuracy
        test_accuracy = 100 * test_correct / test_samples # Testing accuracy
        
        print(f"Epoch: {epoch+1} Train Loss: {train_loss:.3f} Train Accuracy: {train_accuracy:.2f}% " +
              f"Test Loss: {test_loss:.3f} Test Accuracy: {test_accuracy:.2f}%") # Print the results
    
    save_model() # save the model



def save_model():
    """Save the model to the 'model' folder."""
    if not os.path.exists("model"):
        os.makedirs("model")
    torch.save(model.state_dict(), "./model/best_model.pt")

def load_model():
    """Load the model from the 'model' folder."""
    model_path = "./model/best_model.pt"
    if os.path.exists(model_path):
        model.load_state_dict(torch.load(model_path, map_location=device))
        model.eval()
    else:
        print("Saved model not found at", model_path)
        sys.exit(1)

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
    print(f"Predicted class: {classes[predicted.item()]}")

# ----------------- Main (Command-Line Interface) -----------------------
if __name__ == '__main__':
    if len(sys.argv) < 2:
        print("Usage: python classify.py [train | test image_path]")
        sys.exit(1)
    mode = sys.argv[1].lower()
    if mode == "train":
        train()
    elif mode == "test":
        if len(sys.argv) < 3:
            print("Usage: python classify.py test image_path")
            sys.exit(1)
        test_image(sys.argv[2])
    else:
        print("Invalid mode. Use 'train' or 'test'.")
