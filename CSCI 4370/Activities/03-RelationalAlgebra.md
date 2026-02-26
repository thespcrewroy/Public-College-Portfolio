Consider the following simplified schema that contains information about a university. <pk> represents primary key and <fk> represents foreign key. <pk> and <fk> tags are not part of the attribute name.

```
Students(StudentID<pk>, FName, LName, DoB, Major)
Courses(CourseID<pk>, CName, Credits)
Enrollment(EnrollmentID<pk>, StudentID<fk>, CourseID<fk>, grade)
Professors(ProfessorID<pk>, FName, LName, department)
Teaches(TeachID<pk>, ProfessorID<fk>, CourseID<fk>)
```

Answer the following by writing a relational algebra query for each question.

Note: Use the ASCII notation described in the content section on eLC to write your relational algebra expressions.

Make sure to break your queries into multiple lines with intermediate variables. We strongly recommend using only one relational algebra operator per line. This will help you earn partial credit easily. You can lose points if your answers are not clear for the graders.

Q) **Students who have not enrolled in any course. The query result should have StudentID, FName, LName.**

```
SIDsAll   = PROJECT[StudentID](Students)
SIDsEnrl  = PROJECT[StudentID](Enrollment)
SIDsNone  = SIDsAll - SIDsEnrl
SNoneInfo = Students JOIN[Students.StudentID==SIDsNone.StudentID] SIDsNone
Ans1      = PROJECT[StudentID, FName, LName](SNoneInfo)
```

Q) **All students who major in computer science who has received an 'F' grade. The query result should have StudentID, FName, LName.**

```
CS        = SELECT[Major=="computer science"](Students)
FEnroll   = SELECT[grade=="F"](Enrollment)
CSFJoin   = CS JOIN[CS.StudentID==FEnroll.StudentID] FEnroll
Ans2      = PROJECT[StudentID, FName, LName](CSFJoin)
```

Q) **Professors who teach students doing computer science major. The query result should have ProfessorID, FName, LName.**

```
CS        = SELECT[Major=="computer science"](Students)
CSE       = CS JOIN[CS.StudentID==Enrollment.StudentID] Enrollment
CSTeach   = CSE JOIN[CSE.CourseID==Teaches.CourseID] Teaches
CSP       = CSTeach JOIN[CSTeach.ProfessorID==Professors.ProfessorID] Professors
Ans3      = PROJECT[ProfessorID, FName, LName](CSP)
```

Q) **Names of courses that offer the maximum number of credits.**

```
C1 = RENAME[C1](Courses) # compare courses against other courses
C2 = RENAME[C2](Courses) # compare courses against other courses
Pairs = C1 X C2 # creates all possible pairs of courses (ex. 5 courses = 25 comparisons)
Lower = SELECT[C1.Credits < C2.Credits](Pairs) # keeps only pairs where C1 has fewer credits than C2
NotMaxIDs = PROJECT[C1.CourseID](Lower) # extract the IDs of courses that had lower credits than someone else.
NotMax = Courses JOIN[Courses.CourseID==NotMaxIDs.CourseID] NotMaxIDs # reconstructs full course rows for those non-maximum courses
MaxC = Courses - NotMax # Ttke all courses from the origianl and remove the rows that do not have max credits
Ans4 = PROJECT[CName](MaxC) # return only the course names.
```
