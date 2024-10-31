import React from 'react';

const ProblemTypeSelect = ({ problemType, setProblemType }) => (
  <div className="mb-4">
    <label className="block mb-2">Problem Type</label>
    <select
      className="border p-2"
      value={problemType}
      onChange={(e) => setProblemType(e.target.value)}
    >
      <option value="max">Maximize</option>
      <option value="min">Minimize</option>
    </select>
  </div>
);

export default ProblemTypeSelect;
