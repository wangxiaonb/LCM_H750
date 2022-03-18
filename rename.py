import os

path = os.getcwd()+'\\..\\Middlewares\\ST\\threadx\\ports\\cortex_m7\\gnu\\src'

def rename(path):
    for file in os.listdir(path):
        if(file.endswith(".S")):
            print(file)
            newName = file.replace(".S", ".s")  # 这一句的效果是将QL替换为权利
            os.rename(os.path.join(path, file), os.path.join(path, newName))

if __name__ == '__main__':
    rename(path)
