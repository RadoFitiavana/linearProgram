import React from 'react';
import { MathJax } from 'better-react-mathjax';

const ConstraintsTable = ({ numVars, numConstraints, constraints, handleConstraintChange }) => {
  return (
    <table className="table-auto w-full mb-4">
      <thead>
        <tr>
          {Array.from({ length: numVars }).map((_, index) => (
            <th key={index}>
              <MathJax>{`x_${index + 1}`}</MathJax>
            </th>
          ))}
          <th></th>
          <th><MathJax>{"b"}</MathJax></th>
        </tr>
      </thead>
      <tbody>
        {Array.from({ length: numConstraints }).map((_, rowIndex) => (
          <tr key={rowIndex}>
            {Array.from({ length: numVars }).map((_, colIndex) => (
              <td key={colIndex}>
                <input
                  type="number"
                  className="border p-2"
                  onChange={(e) => handleConstraintChange(rowIndex, colIndex, e.target.value)}
                />
              </td>
            ))}
            <td>
              <select className="border p-2">
                <option value="=">=</option>
                <option value=">=">&gt;=</option>
                <option value="<=">&lt;=</option>
              </select>
            </td>
            <td>
              <input
                type="number"
                className="border p-2"
                onChange={(e) => handleConstraintChange(rowIndex, numVars, e.target.value)}
              />
            </td>
          </tr>
        ))}
      </tbody>
    </table>
  );
};

export default ConstraintsTable;
