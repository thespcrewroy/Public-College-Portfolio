/**
 * Copyright (c) 2025 Sami Menik, PhD. All rights reserved.
 * 
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * This software is provided "as is," without warranty of any kind.
 */

package uga.csx370.mydbimpl;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;
import java.util.Set;

import uga.csx370.mydb.Cell;
import uga.csx370.mydb.Predicate;
import uga.csx370.mydb.RA;
import uga.csx370.mydb.Relation;
import uga.csx370.mydb.RelationBuilder;
import uga.csx370.mydb.Type;

/*
* {@code RAImpl}  implements the {@code uga.csx370.mydb.RA} interface, providing 
* concrete implementations of relational algebra operations such as select, project,
* union, intersect, difference, rename, cartesian product, and join.
*/
public class RAImpl implements RA {

    /*
     * The select method takes a relation and a predicate, and returns a new relation
     * containing only the rows from the input relation that satisfy the predicate.
     */
    @Override
    public Relation select(Relation rel, Predicate p) {
        Relation result = new RelationBuilder()
                .attributeTypes(rel.getTypes())
                .attributeNames(rel.getAttrs())
                .build();

        for (int i = 0; i < rel.getSize(); i++) {
            List<Cell> row = rel.getRow(i);
            if (p.check(row)) { // if the predicate is satisfied
                result.insert(row); // insert the row into the result relation
            } // for
        } // for

        return result;
    } // select

    /*
     * The project method takes a relation and a list of attribute names, and returns a new relation
     */
    @Override
    public Relation project(Relation rel, List<String> attrs) {
        for (String attr : attrs) {
            if (!rel.hasAttr(attr)) {
                throw new IllegalArgumentException("Attribute '" + attr + "' not present in relation");
            }
        }

        List<Type> projectedTypes = new ArrayList<>();
        List<Integer> indices = new ArrayList<>();
        for (String attr : attrs) {
            int index = rel.getAttrIndex(attr);
            indices.add(index);
            projectedTypes.add(rel.getTypes().get(index));
        }

        Relation result = new RelationBuilder()
                .attributeTypes(projectedTypes)
                .attributeNames(attrs)
                .build();

        Set<List<Cell>> uniqueRows = new HashSet<>();

        for (int i = 0; i < rel.getSize(); i++) {
            List<Cell> row = rel.getRow(i);
            List<Cell> projectedRow = new ArrayList<>();
            for (int index : indices) {
                projectedRow.add(row.get(index));
            }

            if (uniqueRows.add(projectedRow)) {
                result.insert(projectedRow);
            }
        }

        return result;
    }

    @Override
    public Relation union(Relation rel1, Relation rel2) {
        if (!rel1.getAttrs().equals(rel2.getAttrs()) || !rel1.getTypes().equals(rel2.getTypes())) {    
            throw new IllegalArgumentException("Not union-compatible");
        }
        
        Relation result = new RelationBuilder()
                .attributeNames(rel1.getAttrs())
                .attributeTypes(rel1.getTypes())
                .build();
        
        Set<List<Cell>> seen = new HashSet<>();
        
        for (int i = 0; i < rel1.getSize(); i++) {
            List<Cell> row = rel1.getRow(i);
            if (seen.add(row)) {
                result.insert(row);
            }
        }
        
        for (int i = 0; i < rel2.getSize(); i++) {
            List<Cell> row = rel2.getRow(i);
            if (seen.add(row)) {
                result.insert(row);
            } 
        }
        
        return result;
    }

    @Override
    public Relation intersect(Relation rel1, Relation rel2) {
        // Check union-compatibility (same schema required)
        if (!rel1.getAttrs().equals(rel2.getAttrs()) || !rel1.getTypes().equals(rel2.getTypes())) {
            throw new IllegalArgumentException("Not union-compatible");
        }

        Relation result = new RelationBuilder()
                .attributeNames(rel1.getAttrs())
                .attributeTypes(rel1.getTypes())
                .build();

        // Store all rows from rel2 in a set for lookup
        Set<List<Cell>> rel2Rows = new HashSet<>();
        for (int i = 0; i < rel2.getSize(); i++) {
            rel2Rows.add(rel2.getRow(i));
        }

        // Add rows from rel1 that also exist in rel2 (avoid duplicates)
        Set<List<Cell>> seen = new HashSet<>();
        for (int i = 0; i < rel1.getSize(); i++) {
            List<Cell> row = rel1.getRow(i);
            if (rel2Rows.contains(row) && seen.add(row)) {
                result.insert(row);
            }
        }

        return result;
    }

    @Override
    public Relation diff(Relation rel1, Relation rel2) {
        if (!rel1.getAttrs().equals(rel2.getAttrs()) || !rel1.getTypes().equals(rel2.getTypes())) {
            throw new IllegalArgumentException("Not union-compatible");
        }

        Relation differ = new RelationBuilder()
            .attributeNames(rel1.getAttrs())
            .attributeTypes(rel1.getTypes())
            .build();

        Set<List<Cell>> rel2Copy = new HashSet<>();
        for (int i = 0; i < rel2.getSize(); i++) {
            rel2Copy.add(rel2.getRow(i));
        }

        for (int i = 0; i < rel1.getSize(); i++) {
            List<Cell> temp = rel1.getRow(i);
            if (!rel2Copy.contains(temp)) {
                differ.insert(temp);
            }
        }
        return differ;
    }

    @Override
    public Relation rename(Relation rel, List<String> origAttr, List<String> renamedAttr) {
        if (origAttr.size() != renamedAttr.size()) {
            throw new IllegalArgumentException("origAttr and renamedAttr must have matching counts");
        }

        for (String attr : origAttr) {
            if (!rel.hasAttr(attr)) {
                throw new IllegalArgumentException("Attribute '" + attr + "' not present in relation");
            }
        }

        List<String> newAttrs = new ArrayList<>(rel.getAttrs());
        for (int i = 0; i < origAttr.size(); i++) {
            int index = rel.getAttrIndex(origAttr.get(i));
            newAttrs.set(index, renamedAttr.get(i));
        }

        Relation result = new RelationBuilder()
                .attributeTypes(rel.getTypes())
                .attributeNames(newAttrs)
                .build();

        for (int i = 0; i < rel.getSize(); i++) {
            result.insert(rel.getRow(i));
        }
        return result;
    }

    @Override
    public Relation cartesianProduct(Relation rel1, Relation rel2) {
        for (String attr : rel1.getAttrs()) {
            if (rel2.hasAttr(attr)) {
                throw new IllegalArgumentException(
                        "Relations have common attribute: " + attr);
            }
        }

        List<String> resultAttrs = new ArrayList<>(rel1.getAttrs());
        resultAttrs.addAll(rel2.getAttrs());

        List<Type> resultTypes = new ArrayList<>(rel1.getTypes());
        resultTypes.addAll(rel2.getTypes());

        Relation result = new RelationBuilder()
                .attributeNames(resultAttrs)
                .attributeTypes(resultTypes)
                .build();

        for (int i = 0; i < rel1.getSize(); i++) {
            List<Cell> row1 = rel1.getRow(i);
            for (int j = 0; j < rel2.getSize(); j++) {
                List<Cell> row2 = rel2.getRow(j);
                List<Cell> combinedRow = new ArrayList<>(row1);
                combinedRow.addAll(row2);
                result.insert(combinedRow);
            }
        }

        return result;
    }

    @Override
    public Relation join(Relation rel1, Relation rel2) {
        List<String> attrs1 = rel1.getAttrs();
        List<String> attrs2 = rel2.getAttrs();

        List<String> commonAttrs = new ArrayList<>();
        for (String attr : attrs1) {
            if (rel2.hasAttr(attr)) {
                commonAttrs.add(attr);
            }
        }

        for (String attr : commonAttrs) {
            Type t1 = rel1.getTypes().get(rel1.getAttrIndex(attr));
            Type t2 = rel2.getTypes().get(rel2.getAttrIndex(attr));
            if (t1 != t2) {
                throw new IllegalArgumentException(
                        "Type mismatch on attribute " + attr);
            }
        }

        List<String> resultAttrs = new ArrayList<>(attrs1);
        List<Type> resultTypes = new ArrayList<>(rel1.getTypes());

        for (String attr : attrs2) {
            if (!commonAttrs.contains(attr)) {
                resultAttrs.add(attr);
                resultTypes.add(rel2.getTypes().get(rel2.getAttrIndex(attr)));
            }
        }

        Relation result = new RelationBuilder()
                .attributeNames(resultAttrs)
                .attributeTypes(resultTypes)
                .build();

        for (int i = 0; i < rel1.getSize(); i++) {
            List<Cell> r1 = rel1.getRow(i);
            for (int j = 0; j < rel2.getSize(); j++) {
                List<Cell> r2 = rel2.getRow(j);
                boolean match = true;
                for (String attr : commonAttrs) {
                    Cell c1 = r1.get(rel1.getAttrIndex(attr));
                    Cell c2 = r2.get(rel2.getAttrIndex(attr));
                    if (!c1.equals(c2)) {
                        match = false;
                        break;
                    }
                }

                if (match) {
                    List<Cell> row = new ArrayList<>(r1);
                    for (String attr : attrs2) {
                        if (!commonAttrs.contains(attr)) {
                            row.add(r2.get(rel2.getAttrIndex(attr)));
                        }
                    }
                    result.insert(row);
                }
            }
        }
        return result;
    }

    @Override
    public Relation join(Relation rel1, Relation rel2, Predicate p) {
        for (String attr : rel1.getAttrs()) {
            if (rel2.hasAttr(attr)) {
                throw new IllegalArgumentException(
                        "Relations have common attribute: " + attr);
            }
        }

        Relation cartesian = cartesianProduct(rel1, rel2);
        return select(cartesian, p);
    }
}
