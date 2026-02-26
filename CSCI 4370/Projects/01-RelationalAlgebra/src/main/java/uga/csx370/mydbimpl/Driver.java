/**
 * Copyright (c) 2025 Sami Menik, PhD. All rights reserved.
 * 
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * This software is provided "as is," without warranty of any kind.
 */
package uga.csx370.mydbimpl;

import java.util.Arrays;

import uga.csx370.mydb.RA;
import uga.csx370.mydb.Relation;
import uga.csx370.mydb.RelationBuilder;
import uga.csx370.mydb.Type;

public class Driver {
        public static void main(String[] args) {
                RA ra = new RAImpl();

                Relation student = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.STRING, Type.STRING, Type.INTEGER))
                                .attributeNames(Arrays.asList("ID", "name", "dept_name", "tot_cred"))
                                .build();
                student.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\student_export.csv");

                Relation instructor = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.STRING, Type.STRING, Type.DOUBLE))
                                .attributeNames(Arrays.asList("ID", "name", "dept_name", "salary"))
                                .build();
                instructor.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\instructor_export.csv");

                Relation department = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.STRING, Type.STRING, Type.DOUBLE))
                                .attributeNames(Arrays.asList("dept_name", "building", "budget"))
                                .build();
                department.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\department_export.csv");

                Relation course = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.STRING, Type.STRING, Type.INTEGER))
                                .attributeNames(Arrays.asList("course_id", "title", "dept_name", "credits"))
                                .build();
                course.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\course_export.csv");

                Relation takes = new RelationBuilder()
                                .attributeTypes(
                                                Arrays.asList(Type.INTEGER, Type.INTEGER, Type.INTEGER, Type.STRING,
                                                                Type.INTEGER, Type.STRING))
                                .attributeNames(Arrays.asList("ID", "course_id", "sec_id", "semester", "year", "grade"))
                                .build();
                takes.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\takes_export.csv");

                Relation teaches = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.INTEGER, Type.INTEGER, Type.STRING,
                                                Type.INTEGER))
                                .attributeNames(Arrays.asList("ID", "course_id", "sec_id", "semester", "year"))
                                .build();
                teaches.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\teaches_export.csv");

                Relation section = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.INTEGER, Type.STRING, Type.INTEGER,
                                                Type.STRING, Type.INTEGER, Type.STRING))
                                .attributeNames(Arrays.asList("course_id", "sec_id", "semester", "year", "building",
                                                "room_number", "time_slot_id"))
                                .build();
                section.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\section_export.csv");

                Relation advisor = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.INTEGER))
                                .attributeNames(Arrays.asList("s_ID", "i_ID"))
                                .build();
                advisor.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\advisor_export.csv");

                Relation prereq = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.INTEGER, Type.INTEGER))
                                .attributeNames(Arrays.asList("course_id", "prereq_id"))
                                .build();
                prereq.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\prereq_export.csv");

                Relation classroom = new RelationBuilder()
                                .attributeTypes(Arrays.asList(Type.STRING, Type.INTEGER, Type.INTEGER))
                                .attributeNames(Arrays.asList("building", "room_number", "capacity"))
                                .build();
                classroom.loadData("P1_relational_algebra\\P1_relational_algebra\\mysql-files\\classroom_export.csv");
                System.out.println("=".repeat(80));

                System.out.println("\n" + "=".repeat(80));
                System.out.println(
                                "\nQUERY 1: Find all classrooms with capacity greater than 50 and show what courses were taught in them.");
                System.out.println("MyID: sr42946");
                Relation largeRooms = ra.select(classroom, row -> row.get(2).getAsInt() > 50);

                Relation roomSections = ra.join(largeRooms, section);

                Relation result1 = ra.join(roomSections, course);

                Relation final1 = ra.project(result1,
                                Arrays.asList("building", "room_number", "capacity", "title", "semester", "year"));

                System.out.println("Result:");
                final1.print();

                System.out.println("\n" + "=".repeat(80));
                System.out.println(
                                "\nQUERY 2: Find pairs of instructors within departments where one instructor's salary is at least 2x the salary of the other instructor's salary.");
                System.out.println("MyID: tnl56757");

                Relation instructor1 = ra.rename(
                                instructor,
                                Arrays.asList("ID", "name", "dept_name", "salary"),
                                Arrays.asList("ID1", "name1", "dept1", "salary1"));

                Relation instructor2 = ra.rename(
                                instructor,
                                Arrays.asList("ID", "name", "dept_name", "salary"),
                                Arrays.asList("ID2", "name2", "dept2", "salary2"));

                Relation result2 = ra.join(instructor1, instructor2, row -> {
                        double salary1 = row.get(3).getAsDouble();
                        double salary2 = row.get(7).getAsDouble();
                        Integer id1 = row.get(0).getAsInt();
                        Integer id2 = row.get(4).getAsInt();
                        String dept1 = row.get(2).getAsString();
                        String dept2 = row.get(6).getAsString();

                        return !id1.equals(id2) &&
                                        salary1 >= (salary2 * 2) &&
                                        dept1.equals(dept2);
                });

                Relation final2 = ra.project(result2,
                                Arrays.asList("name1", "salary1", "dept1", "name2", "salary2"));
                System.out.println("Result:");
                final2.print();

                System.out.println("\n" + "=".repeat(80));
                System.out.println(
                                "\nQUERY 3: Find instructors in Computer Science or Biology departments and the courses they teach.");
                System.out.println("MyID: ss18317");

                Relation csBioProfessors = ra.select(instructor, row -> "Comp. Sci.".equals(row.get(2).getAsString()) ||
                                "Biology".equals(row.get(2).getAsString()));
                Relation professorDetails = ra.project(csBioProfessors,
                                Arrays.asList("ID", "name", "dept_name", "salary"));

                Relation teachesSimple = ra.project(teaches, Arrays.asList("ID", "course_id"));
                Relation teachingAssignments = ra.join(professorDetails, teachesSimple);
                Relation courseInfo = ra.project(course, Arrays.asList("course_id", "title"));
                Relation final3 = ra.join(teachingAssignments, courseInfo);
                System.out.println("\nResult: ");
                final3.print();

                System.out.println("\n" + "=".repeat(80));
                System.out.println("\nQUERY 4: Find instructors who have advisees but do not teach courses.");
                System.out.println("MyID: pa38192");

                Relation advisingInstructors = ra.project(ra.rename(
                                ra.project(advisor, Arrays.asList("i_ID")),
                                Arrays.asList("i_ID"),
                                Arrays.asList("ID")), Arrays.asList("ID"));

                Relation teachingInstructors = ra.project(teaches, Arrays.asList("ID"));

                Relation adviseOnly = ra.diff(advisingInstructors, teachingInstructors);
                Relation instructorInfo = ra.project(instructor, Arrays.asList("ID", "name", "dept_name", "salary"));
                Relation final4 = ra.join(adviseOnly, instructorInfo);

                System.out.println("\nResult:");
                final4.print();

                System.out.println("\n" + "=".repeat(80));
                System.out.println(
                                "\nQUERY 5: Find Computer Science students with more than 50 credits who have taken Computer Science courses.");
                System.out.println("MyID: nac55642");

                Relation csStudents = ra.select(student, row -> "Comp. Sci.".equals(row.get(2).getAsString()) &&
                                row.get(3).getAsInt() > 50);
                Relation csStudentIds = ra.project(csStudents, Arrays.asList("ID"));
                Relation csCourses = ra.select(course, row -> "Comp. Sci.".equals(row.get(2).getAsString()));
                Relation csCourseIds = ra.project(csCourses, Arrays.asList("course_id"));
                Relation csEnrollments = ra.join(csCourseIds, takes);
                Relation studentsWithCsCourses = ra.project(csEnrollments, Arrays.asList("ID"));
                Relation csStudentsWithCsCourses = ra.intersect(csStudentIds, studentsWithCsCourses);

                Relation csStudentInfo = ra.project(csStudents, Arrays.asList("ID", "name", "dept_name", "tot_cred"));
                Relation final5 = ra.join(csStudentsWithCsCourses, csStudentInfo);
                System.out.println("\nResult: ");
                final5.print();
        }
}