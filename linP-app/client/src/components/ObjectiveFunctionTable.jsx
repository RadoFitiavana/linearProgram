import React from 'react';
import { MathJax } from 'better-react-mathjax';

const ObjectiveFunctionTable = ({ numVars, objectiveFunction, setObjectiveFunction }) => (
  <div className="mb-4">
    <h3>Objective Function</h3>
    <table className="table-auto w-full">
      <thead>
        <tr>
          {Array.from({ length: numVars }).map((_, index) => (
            <th key={index}>
              <MathJax>{`c_${index + 1}`}</MathJax>
            </th>
          ))}
        </tr>
      </thead>
      <tbody>
        <tr>
          {Array.from({ length: numVars }).map((_, index) => (
            <td key={index}>
              <input
                type="number"
                className="border p-2"
                onChange={(e) =>
                  setObjectiveFunction({ ...objectiveFunction, [index]: e.target.value })
                }
              />
            </td>
          ))}
        </tr>
      </tbody>
    </table>
  </div>
);

export default ObjectiveFunctionTable;
