# UCalgary Mixed Reality (UCXR)

This is the Universty of Calgary's Mixed Reality design team repository. Here, all of our code for every sensor, GPU API, machine learning algorithm, etc. will be stored for version control.

## Branches

We will create a branch for every single project. This means that a new branch will be needed for Gesture Recognition, GPU and Microdisplay code, tracking algorithms, data processing, and other components of this project. When starting your own project, please create a new branch and use a descriptive name.

Also add the .gitignore file located in this branch to every new branch you create. This will prevent vscode/vs settings from being added to every single commit.

## Standards

We will have a couple of coding standards to help keep our code readable and scalable.

- Use Camel Case
- Use as few lines as possible
- Try and make functions as small as possible. Use multiple functions to carry out one big task, rather than one big function.
- For all segments of code that are repeated, try and make a function to automate that task.
- Avoid nesting your loops. The more nested loops you have in your code, the less efficient the code will be.
- Capitalize all class names, and keep all variable and function names lowercase.
- Comment on a lot of your code to help keep it readable. Create your comments so that they are helpful for debugging later down the line.

```
// Example:

int myVariable1;
char myVariable2;

void doSomething(*a){
  *a++;
}

class School{

  private:
    int studentNames;
    int studentGrades;

  public:
    int getStudentNames(){
      return studentNames;
    }
    int getStudentGrades{
      return studentGrades;
    }
    int studentCount;

};

```

## Contributing

Before you merge your branch with any other branches, always get your code reviewed by a peer and a contributor of the branch you are trying to merge to. Get them to comment on your pull request, and then proceed to merge.

Please make sure to test all of your code before merging.

## For more info:

Website:
[UCXR Website](https://uc-mixedreality.ca/)

Linkedin:
[UCXR Linkedin](https://www.linkedin.com/company/ucxr/)

Instagram:
[UCXR Instagram](https://www.instagram.com/uofc_xr/)

Operated by the University of Calgary and the Schulich School of Engineering
